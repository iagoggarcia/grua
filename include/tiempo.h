#ifndef TIEMPO_H
#define TIEMPO_H

// Calcula el tiempo transcurrido entre el frame actual y el anterior.
// Recibe por referencia el instante del último frame para poder actualizarlo.
float lapsoDeltaTime(float& ultimoFrame);

#endif