#include "Aurora.h"
#include "Arduino.h"

/*
 * Contructor to set GPIOs, write range and start the transitions queue;
 */

Aurora::Aurora(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint32_t resolution) {
    this->redPin = redPin;
    this->greenPin = greenPin;
    this->bluePin = bluePin;
    this->resolution = clamp(resolution, 1, INT32_MAX);
    analogWriteRange(this->resolution);
    this->reset();
}

/* 
 * Sends data to GPIOs to set the color of LEDs to the RGB argument.
 */
void Aurora::setColor(uint16_t red, uint16_t green, uint16_t blue){
    this->red = clamp(red, 0 , this->resolution);
    this->green = clamp(green, 0 , this->resolution);
    this->blue = clamp(blue, 0 , this->resolution);

    analogWrite(this->redPin, this->red);
    analogWrite(this->greenPin, this->green);
    analogWrite(this->bluePin, this->blue);
}

/*
 * Clamps the value to inferior and superior limits. This method is used to avoid incorrects values to be inputed in some
 * of the internal methods.
 * If value is lesser than lower or greater than upper, returns lower or upper respectively, otherwise returns value.
 */ 
uint64_t Aurora::clamp(uint64_t value, uint64_t lower, uint64_t upper){
    return max(lower, min(value, upper));
}

/* 
 * Adds a color change to transitions queue.
 * If the transitions queue is empty, the selected color will be displayed statically 
 * and no other transition will happen. 
 * In case of the queue has another transitions, the selected color will be displayed after the
 * the last transition is executed, and will last for pause time in milliseconds. After that, the next
 * transition will be exeuted.
 */ 
void Aurora::addColor(uint16_t red, uint16_t green, uint16_t blue, uint64 pause){
    this->addTransition(red, green, blue, 1, 0, pause);
}

/* 
 * Clear all transitions and adds a color change to transitions queue.
 * As this method clear the transitions queue, it will stop any color transition.
 * The selected color will be static, until other transitions are added.
 * As this method doesn't add a pause time, if another transition is added, 
 * this color may not be displayed, to solve that, you can call the pause method before
 * adding another transition.
 */ 
void Aurora::changeColor(uint16_t red, uint16_t green, uint16_t blue){
    this->clear();
    this->addColor(red, green, blue, 0);
}

/* 
 * Fades gradually from a RGB color to another, based on duration and using the maximum gradient.
 * This method clear all transitions and adds two transitions, one for each color.
 * As this method clear the transitions queue, it will stop the previous transitions
 * and will only fade between the arguments colors, until other transitions are added.
 */ 
void Aurora::fade(uint16_t fromRed, uint16_t fromGreen, uint16_t fromBlue, uint16_t toRed, uint16_t toGreen, uint16_t toBlue, uint64_t duration) {
    this->fade(fromRed, fromGreen, fromBlue, toRed, toGreen, toBlue, this->resolution, duration);
}

/* 
 * Fades gradually from a RGB color to another, based on the gradient and duration.
 * This method clear all transitions, adds two transitions, one for each color
 * As this method clear the transitions queue, it will stop the previous transitions
 * and will only fade between the arguments colors, until other transitions are added.
 */ 
void Aurora::fade(uint16_t fromRed, uint16_t fromGreen, uint16_t fromBlue, uint16_t toRed, uint16_t toGreen, uint16_t toBlue, uint32_t gradient, uint64_t duration) {
    this->clear();
    this->setColor(fromRed, fromGreen, fromBlue);
    this->addTransition(toRed, toGreen, toBlue, gradient, duration, 0);
    this->addTransition(fromRed, fromGreen, fromBlue, gradient, duration, 0);
}

/* 
 * Adds a transition to the queue that fades gradually from the last transition color 
 * to the selected one, based on duration and using the maximum gradient.
 */ 
void Aurora::addFade(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration) {
    this->addFade(red, green, blue, this->resolution, duration);
}

/* 
 * Adds a transitions to the queue that fades gradually from the last transitions color 
 * to the selected one, based on the gradient and duration.
 */ 
void Aurora::addFade(uint16_t red, uint16_t green, uint16_t blue, uint32_t gradient, uint64_t duration){
    this->addTransition(red, green, blue, gradient, duration, 0);
}

/* 
 * Adds a transition that fades gradually from black(off)
 * to selected rgb color, based on duration and using the maximum gradient.
 */
void Aurora::fadeIn(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration) {
    fadeIn(red, green, blue, this->resolution, duration);
}

/* 
 * Adds a transition that fades gradually from black(off)
 * to selected rgb color, based on gradient and duration.
 */
void Aurora::fadeIn(uint16_t red, uint16_t green, uint16_t blue, uint32_t gradient, uint64_t duration) {
    this->clear();
    this->setColor(0, 0, 0);
    this->addTransition(red, green, blue, gradient, duration, 0);
}

/* 
 * Adds a transition that fades gradually from selected color to black(off),
 * based on duration and using the maximum gradient.
 */
void Aurora::fadeOut(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration) {
    fadeOut(red, green, blue, this->resolution, duration);
}

/* 
 * Adds a transition that fades gradually from selected color to black(off),
 * based on gradient and duration.
 */
void Aurora::fadeOut(uint16_t red, uint16_t green, uint16_t blue, uint32_t gradient, uint64_t duration) {
    this->clear();
    this->setColor(red, green, blue);
    this->addTransition(0, 0, 0, gradient, duration, 0);
}

/*
 * Adds a transition commands to queue
 */
void Aurora::addTransition(uint16_t red, uint16_t green, uint16_t blue, uint32_t gradient, uint64_t duration, uint64_t pause){

    /* Colors cannot have values greater than resolution or lower than 0 */
    red = this->clamp(red, 0, this->resolution);
    green = this->clamp(green, 0, this->resolution);
    blue = this->clamp(blue, 0, this->resolution);

    /* Difference from previous color to new color */
    int redDifference;
    int greenDifference;
    int blueDifference;

    /* If there are no commands, then use the actual rgb values */
    if(commands.empty()) {
        redDifference = red - this->red;
        greenDifference = green - this->green;
        blueDifference = blue - this->blue;
    } else {
        redDifference = red - commands.back().red;
        greenDifference = green - commands.back().green;
        blueDifference = blue - commands.back().blue;
    }

    /* Calculates the maximum gradient */
    int maxGradient = max(abs(redDifference), max(abs(greenDifference), abs(blueDifference)));

    /* 
     * If the gradient is zero, then it means that there is no difference in previous color and the new one,
     * and because of that, nothing is done.
     */
    if(clamp(gradient, 0, maxGradient) == 0) {
        return;
    }

    /* Coefficient used to calculate each gradient color */
    double redCoefficient = redDifference / (double)gradient;
    double greenCoefficient = greenDifference / (double)gradient;
    double blueCoefficient = blueDifference / (double)gradient;

    /* Adds the new command to vector */
    commands.emplace_back(TransitionCommand(red, green, blue, redCoefficient, greenCoefficient, blueCoefficient, gradient, duration, pause));
}

/* 
 * Adds a set of transitions that change between black and color 
 * every period of time, in miliseconds.
 */
void Aurora::blink(uint16_t red, uint16_t green, uint16_t blue, uint64_t period){
    this->clear();
    this->setColor(red, green, blue);
    this->addTransition(0, 0, 0, 1, period);
    this->addTransition(red, green, blue, 1, period);
}

/* 
 * Keeps the color of last transition for time, in mileseconds.
 */
void Aurora::pause(uint64_t time){
    if(!commands.empty()){
        commands.back().pause = time;
    }
}

/* 
 * Executes the transistion of index, after the last transition is executed, for x times.
 */
void Aurora::jumpTo(uint16_t index, uint16_t times) {
    if(index <= commands.size() - 1){
        commands.back().jumpTo = index;
        commands.back().jumpFor = times;
    }
}

/* Executes the transition command chain. */
void Aurora::execute() {
    /* There is no commands to execute, do nothing. */
    if (commands.empty()) {
        return;
    }

    /* First execution, setting up start time and transition index*/
    if (startTime == 0) {
        startTime = millis();
        transitionIndex = 1;
    }

    TransitionCommand & command = commands.at(commandIndex);

    /* Checking if the transition was executed until the end of its duration */
    if (millis() > startTime + command.duration + command.pause) {
        setColor((uint16_t) command.red,
                 (uint16_t) command.green,
                 (uint16_t) command.blue);

        transitionIndex = 1;
        startTime = 0;

        /* 
         * If a transition has a jumpTo index, the next transition to be executed should be 
         * the one of that index.
         */
        if(command.jumpTo != emptyJumpTo){
            /* Checking if the transition jumped back enough times */
            if(command.jumpCounter < command.jumpFor){
                commandIndex = command.jumpTo;
                command.jumpCounter++;
                return;
            } else {
                command.jumpCounter = 0;
            }
        }
        
        if (commandIndex < (commands.size() - 1)) {
            /* Checking if it has more transistions to execute, otherwise go back to first */
            /* Go to next transition */
            commandIndex++;
        } else {
            /* Executed the last transition, go back to first one */
            commandIndex = 0;
        }
        return;
    }

    /* Checking if it is the time to change to next gradient color */
    if (millis() > (startTime + (command.duration / command.gradient * transitionIndex)) && transitionIndex <= command.gradient) {
        setColor((uint16_t) (ceil(command.red - ((command.gradient - transitionIndex) * command.redCoefficient))),
                 (uint16_t) (ceil(command.green - ((command.gradient - transitionIndex) * command.greenCoefficient))),
                 (uint16_t) (ceil(command.blue - ((command.gradient - transitionIndex) * command.blueCoefficient))));
        transitionIndex++;
    }
}

/* Returns information about status of transitions, actual rgb color, etc */
void Aurora::info() {
    for(unsigned int i =0; i < commands.size(); i++){
        Serial.printf("index: %d - %d, %d, %d - %d, %lld %lld\n", i, commands[i].red, commands[i].green, commands[i].blue, commands[i].gradient, commands[i].duration, commands[i].pause);
    }
}

/* Clear all transitions from vector and reset status */
void Aurora::clear(){
    commands.clear();
    startTime = 0;
    transitionIndex = 1;
    commandIndex = 0;
}

/* Reset status and change the lights to default color */
void Aurora::reset(){
    this->clear();
    this->setColor(defaultRed, defaultGreen, defaultBlue);
}
