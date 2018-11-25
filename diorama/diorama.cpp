#include <FastLED.h>

static const int NLEDS = 100;
CRGB leds[NLEDS];

static const int DATA_PIN = 5;

class Scene {
public:
    CHSV sky;
    CHSV water;
    CHSV ground;

    Scene(long sky, long water, long ground)
        :
        sky(rgb2hsv_approximate(sky)),
        water(rgb2hsv_approximate(water)),
        ground(rgb2hsv_approximate(ground))
    { }

    Scene(CHSV &sky, CHSV &water, CHSV &ground)
        : sky(sky), water(water), ground(ground)
    {}

    Scene(Scene &s1, Scene &s2, fract8 alpha)
        :   sky(blend(s1.sky, s2.sky, alpha)),
            water(blend(s1.water, s2.water, alpha)),
            ground(blend(s1.ground, s2.ground, alpha))
        {}
};

Scene white(0xffffff, 0xffffff, 0xffffff);
Scene day(0x59a8ff, 0x1f604a, 0xff8080);
Scene dusk(0x371b5b, 0x0e242b, 0xb28b55);
Scene night(0x0e0e26, 0x000000, 0xbbffff);

int times[] = {0, 20, 30, 40, 50, 60, 70, 80};
Scene scenes[] = {white, white, day, day, dusk, night, night, white};
int NEVENTS = sizeof(times) / sizeof(times[0]);

Scene get_scene(float t)
{
    if (t <= times[0]) {
        return scenes[0];
    }
    if (t >= times[NEVENTS - 1]) {
        return scenes[NEVENTS - 1];
    }

    int i1, i2 = 1;
    while (times[i2] <= t) {
        i2++;
    }
    i1 = i2 - 1;

    float alpha = (float)(t - times[i1]) / (float)(times[i2] - times[i1]);
    fract8 b = floor(alpha * 256);
    return Scene(scenes[i1], scenes[i2], b);
}

void render(CHSV &sky, CHSV &water, CHSV &ground)
{

    for (int i = 1; i <= 16; i++) {
        leds[i] = ground;
    }

    for (int i = 26; i <= 41; i++) {
        leds[i] = water;
    }

    for (int i = 50; i <= 65; i++) {
        leds[i] = sky;
    }
}

void render(Scene &scene)
{
    render(scene.sky, scene.water, scene.ground);
}


void setup()
{
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NLEDS);
	delay(100);
}

void loop() {
    static float t;
    const float step = 0.1;

    Scene s = get_scene(t);
    render(s);
    FastLED.show();

    t += step;
    if (t > times[NEVENTS - 1]) {
        t -= times[NEVENTS - 1];
    }

    delay((long)round(step * 1000.0));
}
