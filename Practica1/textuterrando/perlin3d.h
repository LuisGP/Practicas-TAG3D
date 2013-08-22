/*
 *  perlin3d.h
 *  Practica1
 *
 *  Converted from Java by Alejandro Ribao and Luis Gasco on 17/03/08.
 *
 */
 
 /*****************************************************************************
 *                        J3D.org Copyright (c) 2000
 *                               Java Source
 *
 * This source is licensed under the GNU LGPL v2.1
 * Please read http://www.gnu.org/copyleft/lgpl.html for more information
 *
 * This software comes with the standard NO WARRANTY disclaimer for any
 * purpose. Use it at your own risk. If there's a problem you get to fix it.
 *
 ****************************************************************************/


#include <math.h>
#include <iostream>
#include <cstdlib>

using namespace std;

void PerlinNoiseGenerator(unsigned int seed);
double improvedNoise(double x, double y, double z);
float noise1(float x);
float noise2(float x, float y);
float noise3(float x, float y, float z);
double imporvedTurbulence(double x, double y, double z, float loF, float hiF);
float turbulence2(float x, float y, float freq);
float turbulence3(float x, float y, float z, float freq);
float tileableNoise1(float x, float w);
float tileableNoise2(float x, float y, float w, float h);
float tileableNoise3(float x, float y, float z, float w, float h, float d);
float tileableTurbulence2(float x, float y, float w, float h, float freq);
float tileableTurbulence3(float x, float y, float z, float w, float h, float d, float freq);
double lerp(double t, double a, double b);
float lerp(float t, float a, float b);
double fade(double t);
double grad(int hash, double x, double y, double z);
float bias(float a, float b);
float gain(float a, float b);
float sCurve(float t);
void normalize2(float* v);
void normalize3(float* v);
void initPerlin1();


