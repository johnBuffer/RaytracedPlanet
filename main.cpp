#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

double pi = 3.1415926;

int main()
{
    sf::RenderWindow window(sf::VideoMode(900, 900), "Planisphere");
    window.setFramerateLimit(6000);

    sf::Shader shader, blur;

    const std::string str_blur_h = "uniform sampler2D image;uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);void main(){vec2 TexCoords = gl_TexCoord[0].xy; vec2 tex_offset = 1.0 / textureSize(image, 0); vec3 result = texture(image, TexCoords).rgb * weight[0]; for(int i = 1; i < 5; ++i){result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];}gl_FragColor = vec4(result, 1.0);}";
    const std::string str_blur_v = "uniform sampler2D image;uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);void main(){vec2 TexCoords = gl_TexCoord[0].xy; vec2 tex_offset = 1.0 / textureSize(image, 0); vec3 result = texture(image, TexCoords).rgb * weight[0]; for(int i = 1; i < 5; ++i){result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];}gl_FragColor = vec4(result, 1.0);}";
    sf::Shader blur_h, blur_v;
    blur_h.loadFromMemory(str_blur_h, sf::Shader::Fragment);
    blur_v.loadFromMemory(str_blur_v, sf::Shader::Fragment);

    if (!shader.loadFromFile("shader.frag", sf::Shader::Fragment))
    {
        // error...
    }

    if (!blur.loadFromFile("blurShader.frag", sf::Shader::Fragment))
    {
        // error...
    }

    sf::RenderTexture screen, blurTex;
    screen.create(900, 900);
    blurTex.create(900, 900);

    sf::Texture mapLight, mapColor, sun;
    mapColor.loadFromFile("mapColor.jpg");
    mapLight.loadFromFile("mapLight.bmp");
    sun.loadFromFile("sun2.jpg");
    mapColor.setRepeated(true);

    sf::Sprite sprite(mapColor);
    sf::Sprite screenSprite(screen.getTexture());
    sprite.scale(900/2048.0, 900/1024.0);

    shader.setParameter("mapLight", mapLight);
    shader.setParameter("mapColor", mapColor);
    shader.setParameter("lightTex", sun);

    double rotX(0), rotY(0), time(-1.69), distToPlanet(2);

    while (window.isOpen())
    {
        double timeSpeed = 0.015;
        if(sin(time) > 0.99) timeSpeed = 0.0025;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
            rotX -= 0.02;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            rotX += 0.02;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            rotY -= 0.02;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            rotY += 0.02;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
            time -= timeSpeed;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
            time += timeSpeed;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            distToPlanet -= 0.01;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            distToPlanet += 0.01;
        }

        shader.setParameter("rotX", rotX);
        shader.setParameter("rotY", rotY);
        shader.setParameter("time", time);
        shader.setParameter("distToPlanet", distToPlanet);

        screen.clear();
        screen.draw(sprite, &shader);
        screen.display();

        window.clear();

        window.draw(screenSprite);

        if (sin(time) > 0.99)
        {
            sf::Sprite spriteBloom(screen.getTexture());
            spriteBloom.setScale(0.1, 0.1);
            blurTex.draw(spriteBloom);
            sf::Sprite spriteBloomRenderer(blurTex.getTexture());
            for (int i(0); i<2; ++i)
            {
                blurTex.draw(spriteBloomRenderer, &blur_h);
                blurTex.draw(spriteBloomRenderer, &blur_v);
            }
            blurTex.display();
            spriteBloomRenderer.scale(10, 10);

            window.draw(spriteBloomRenderer, sf::BlendAdd);
        }

        window.display();
    }

    return 0;
}
