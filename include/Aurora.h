#ifndef AURORA_H
#define AURORA_H
#include <Arduino.h>

class Aurora {
public:
    Aurora(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint32_t writeRange);
    
    void addColor(uint16_t red, uint16_t green, uint16_t blue);
    void addColor(uint16_t red, uint16_t green, uint16_t blue, uint64 pause);
    void changeColor(uint16_t red, uint16_t green, uint16_t blue);
    
    void addFade(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration);
    void addFade(uint16_t red, uint16_t green, uint16_t blue, uint32_t gradient, uint64_t duration);

    void fade(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration);
    void fade(uint16_t red, uint16_t green, uint16_t blue, uint32_t gradient, uint64_t duration);
    
    void fadeIn(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration);
    void fadeIn(uint16_t red, uint16_t green, uint16_t blue, uint32_t gradient, uint64_t duration);
    
    void fadeOut(uint16_t red, uint16_t green, uint16_t blue, uint64_t duration);
    void fadeOut(uint16_t red, uint16_t green, uint16_t blue, uint32_t gradient, uint64_t duration);
    
    void blink(uint16_t red, uint16_t green, uint16_t blue, uint64_t period);

    void pause(uint64_t time);

    void jumpTo(uint16_t transitionIndex, uint16_t times);

    void info();
    void execute();
    void reset();

private:
    class TransitionCommand {
    public:
        uint16_t red = 0, green = 0, blue = 0;
        double redCoefficient = 0, greenCoefficient = 0, blueCoefficient = 0;
        uint32_t gradient = 1;
        uint64_t duration = 0;
        uint64_t pause = 0;
        uint16_t jumpTo = emptyJumpTo;
        uint16_t jumpFor = 0;
        uint16_t jumpCounter = 0;

        TransitionCommand(uint16_t red, uint16_t green, uint16_t blue, double redCoefficient, double greenCoefficient, double blueCoefficient, uint16_t gradient, uint64_t duration) {
            this->red = red;
            this->green = green;
            this->blue = blue;
            this->redCoefficient = redCoefficient;
            this->greenCoefficient = greenCoefficient;
            this->blueCoefficient = blueCoefficient;
            this->gradient = gradient;
            this->duration = duration;
        }
        TransitionCommand(uint16_t red, uint16_t green, uint16_t blue) {
            TransitionCommand(red, green, blue, 0, 0, 0, 1, 0);
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

    void addTransition(uint16_t red, uint16_t green, uint16_t blue, uint32_t gradient, uint64_t duration);
    void setColor(uint16_t red, uint16_t green, uint16_t blue);
    uint64_t clamp(uint64_t value, uint64_t lower, uint64_t upper);
    void clear();
    static const uint16 emptyJumpTo = UINT16_MAX; /* Used as 'null' to jumpTo variable*/
};

#endif //AURORA_H
