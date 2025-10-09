#ifndef TRANSFORM_H
#define TRANSFORM_H

typedef struct
{
float ia;
float ib;
float ic;

} clarkeInput_t;

typedef struct
{
float ialpha ;
float ibeta;


} clarkeOutput_t;


typedef struct
{
float Va;
float Vb;
float Vc;
} inv_clarkeOutput_t;

typedef struct 
{
    float Ialpha;
    float Ibeta;
    float cos;
    float sin;


} parkInput_t;

typedef struct 
{
    float Id;
    float Iq;

} parkOutput_t;

typedef struct 
{
    float Vd;
    float Vq;
    float cos;
    float sin;

} inv_parkInput_t;

typedef struct 
{
    float Valpha;
    float Vbeta;

} inv_parkOutputInvClarkeIn_t;




// Clarke Transform (abc → αβ)
void clarke (clarkeInput_t* phaseCurrents, clarkeOutput_t* ab_currents );

// Park Transform (αβ → dq)
void park(parkInput_t* ab_currents, parkOutput_t* dq_currents);

// Inverse Park Transform (dq → αβ)

void inv_park(inv_parkInput_t* dq_voltages, inv_parkOutputInvClarkeIn_t* ab_voltages);

// Inverse Clarke (αβ → abc)
void inv_clarke(inv_parkOutputInvClarkeIn_t *ab_voltages, inv_clarkeOutput_t* phaseVoltages);



#endif //TRANSFORM_H