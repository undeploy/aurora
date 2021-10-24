#ifndef AURORA_H
#define AURORA_H
#include <Arduino.h>

class Transition {
    public:
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint32_t steps;
    uint64_t duration;
    Transition(uint16_t red, uint16_t green, uint16_t blue, uint32_t steps, uint64_t duration){
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->steps = steps;
        this->duration = duration;
    }
    Transition(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration){
        Transition(red, green, blue, 1, duration);
    }
    Transition(uint16_t red, uint16_t green, uint16_t blue){
        Transition(red, green, blue, 1, 0);
    }
    Transition()=default;
};

class Aurora {
public:
    Aurora(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint32_t writeRange);

    void addTransition(uint16_t red, uint16_t green, uint16_t blue);
    void addTransition(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration);
    void addTransition(uint16_t red, uint16_t green, uint16_t blue, uint32_t steps, uint64_t duration);
    void addTransition(Transition transition);
    
    void changeColor(uint16_t red, uint16_t green, uint16_t blue);
    void changeColor(uint16_t red, uint16_t green, uint16_t blue, uint64_t delay);
    
    void fade(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration);
    void fade(uint16_t red, uint16_t green, uint16_t blue, uint32_t steps, uint64_t duration);
    
    void fadeIn(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration);
    void fadeIn(uint16_t red, uint16_t green, uint16_t blue, uint32_t steps, uint64_t duration);
    
    void fadeOut(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration);
    void fadeOut(uint16_t red, uint16_t green, uint16_t blue, uint32_t steps, uint64_t duration);
    
    void blink(uint16_t red, uint16_t green, uint16_t blue, uint64_t period);

    void info();
    void execute();
    void reset();

private:
    class TransitionCommand {
    public:
        uint16_t red = 0, green = 0, blue = 0;
        double redCoefficient = 0, greenCoefficient = 0, blueCoefficient = 0;
        uint32_t steps = 0;
        uint64_t duration = 0;
        TransitionCommand(uint16_t red, uint16_t green, uint16_t blue, double redCoefficient, double greenCoefficient, double blueCoefficient, uint16_t steps, uint64_t duration){
            this->red = red;
            this->green = green;
            this->blue = blue;
            this->redCoefficient = redCoefficient;
            this->greenCoefficient = greenCoefficient;
            this->blueCoefficient = blueCoefficient;
            this->steps = steps;
            this->duration = duration;
        }
        TransitionCommand(uint16_t red, uint16_t green, uint16_t blue){
            this->red = red;
            this->green = green;
            this->blue = blue;
            this->steps = 1;
        }
        TransitionCommand()=default;
    };
    uint16_t defaultRed = 50;
    uint16_t defaultGreen = 0;
    uint16_t defaultBlue = 0;
    
    std::vector<TransitionCommand> commands;
    uint8_t redPin, greenPin, bluePin;
    uint16_t red, green, blue;
    uint16_t transitionIndex = 0;
    uint32_t writeRange;
    uint16_t commandIndex = 0;
    uint64_t startTime = 0;

    void setColor(uint16_t red, uint16_t green, uint16_t blue);
    uint64_t clamp(uint64_t value, uint64_t lower, uint64_t upper);
    void clear();
};

#endif //AURORA_H
