#include <math.h>

#include "r_local.h"
#include "p_local.h"
#include "d_main.h"

//
// R_LightGetHSV
// Set HSV values based on given RGB
//

void R_LightGetHSV(int r, int g, int b, int *h, int *s, int *v)
{
    int min = r;
    int max = r;
    float delta = 0.0f;
    float j = 0.0f;
    float x = 0.0f;
    float xr = 0.0f;
    float xg = 0.0f;
    float xb = 0.0f;
    float sum = 0.0f;
    
    if(g < min) min = g;
    if(b < min) min = b;
    
    if(g > max) max = g;
    if(b > max) max = b;
    
    delta = ((float)max / 255.0f);
    
    if(dfcmp(delta, 0.0f))
        delta = 0;
    else
    {
        j = ((delta - ((float)min / 255.0f)) / delta);
    }
    
    if(!dfcmp(j, 0.0f))
    {
        xr = ((float)r / 255.0f);
        
        if(!dfcmp(xr, delta))
        {
            xg = ((float)g / 255.0f);
            
            if(!dfcmp(xg, delta))
            {
                xb = ((float)b / 255.0f);
                
                if(dfcmp(xb, delta))
                {
                    sum = ((((delta - xg) / (delta - (min / 255.0f))) + 4.0f) -
                        ((delta - xr) / (delta - (min / 255.0f))));
                }
            }
            else
            {
                sum = ((((delta - xr) / (delta - (min / 255.0f))) + 2.0f) -
                    ((delta - (b / 255.0f)) /(delta - (min / 255.0f))));
            }
        }
        else
        {
            sum = (((delta - (b / 255.0f))) / (delta - (min / 255.0f))) -
                ((delta - (g / 255.0f)) / (delta - (min / 255.0f)));
        }
        
        x = (sum * 60.0f);
        
        if(x < 0)
            x += 360.0f;
    }
    else
        j = 0.0f;
    
    *h = (int)((x / 360.0f) * 255.0f);
    *s = (int)(j * 255.0f);
    *v = (int)(delta * 255.0f);
}

//
// R_LightGetRGB
// Set RGB values based on given HSV
//

void R_LightGetRGB(int h, int s, int v, int *r, int *g, int *b)
{
    float x = 0.0f;
    float j = 0.0f;
    float i = 0.0f;
    int table = 0;
    float xr = 0.0f;
    float xg = 0.0f;
    float xb = 0.0f;
    
    j = (h / 255.0f) * 360.0f;
    
    if(360.0f <= j)
        j -= 360.0f;
    
    x = (s / 255.0f);
    i = (v / 255.0f);
    
    if(!dfcmp(x, 0.0f))
    {
        table = (int)(j / 60.0f);
        if(table < 6)
        {
            float t = (j / 60.0f);
            switch(table)
            {
            case 0:
                xr = i;
                xg = ((1.0f - ((1.0f - (t - (float)table)) * x)) * i);
                xb = ((1.0f - x) * i);
                break;
            case 1:
                xr = ((1.0f - (x * (t - (float)table))) * i);
                xg = i;
                xb = ((1.0f - x) * i);
                break;
            case 2:
                xr = ((1.0f - x) * i);
                xg = i;
                xb = ((1.0f - ((1.0f - (t - (float)table)) * x)) * i);
                break;
            case 3:
                xr = ((1.0f - x) * i);
                xg = ((1.0f - (x * (t - (float)table))) * i);
                xb = i;
                break;
            case 4:
                xr = ((1.0f - ((1.0f - (t - (float)table)) * x)) * i);
                xg = ((1.0f - x) * i);
                xb = i;
                break;
            case 5:
                xr = i;
                xg = ((1.0f - x) * i);
                xb = ((1.0f - (x * (t - (float)table))) * i);
                break;
            }
        }
    }
    else
        xr = xg = xb = i;
    
    *r = (int)(xr * 255.0f);
    *g = (int)(xg * 255.0f);
    *b = (int)(xb * 255.0f);
}

//
// R_SetLightFactor
//

void R_SetLightFactor(float lightfactor)
{
    int l;
    light_t* light;
    float f;
    
    f = lightfactor / 100.0f;
    
    for(l = 0; l < numlights; l++)
    {
        light = &lights[l];
        
        if(l >= 256)
        {
            int h, s, v;
            int r, g, b;
            
            R_LightGetHSV(light->r, light->g, light->b, &h, &s, &v);
            
            v = MIN((int)((float)v * f), 255);
            
            R_LightGetRGB(h, s, v, &r, &g, &b);

            light->base_r = r;
            light->base_g = g;
            light->base_b = b;
        }
        else
            light->base_r = light->base_g = light->base_b = MIN((int)((float)l * f), 255);

        light->active_r = light->base_r;
        light->active_g = light->base_g;
        light->active_b = light->base_b;
    }
}

//
// R_RefreshBrightness
//

void R_RefreshBrightness(void)
{
    float factor;
    float brightness = 100;

    factor = (((infraredFactor > brightness) ? infraredFactor : brightness) + 100.0f);
    R_SetLightFactor(factor);
}


