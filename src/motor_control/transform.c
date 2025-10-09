#include <math.h>
#include "transform.h"



// Clarke Transform (abc → αβ)
void clarke (clarkeInput_t* phaseCurrents, clarkeOutput_t* ab_currents ) {
    ab_currents->ialpha = phaseCurrents->ia;
    ab_currents->ibeta = (1/(sqrt(3)))* (phaseCurrents->ia + 2* phaseCurrents->ib);

}

// Park Transform (αβ → dq)
void park(parkInput_t* ab_currents, parkOutput_t* dq_currents) {

    dq_currents->Id = ab_currents->Ialpha * ab_currents->cos + ab_currents->Ibeta * ab_currents->sin;
    dq_currents->Iq = ab_currents->Ibeta * ab_currents->cos - ab_currents->Ialpha * ab_currents->sin;

}


// Inverse Clarke (αβ → abc)
void inv_clarke(inv_parkOutputInvClarkeIn_t *ab_voltages, inv_clarkeOutput_t* phaseVoltages) {

    phaseVoltages->Va = ab_voltages->Valpha;
    phaseVoltages->Vb = (-ab_voltages->Valpha + sqrt(3)*ab_voltages->Vbeta)/2;
    phaseVoltages->Vc = (-ab_voltages->Valpha - sqrt(3)*ab_voltages->Vbeta)/2;

}

// Inverse Park Transform (dq → αβ)
void inv_park(inv_parkInput_t* dq_voltages, inv_parkOutputInvClarkeIn_t* ab_voltages) {

    ab_voltages->Valpha = dq_voltages->Vd * dq_voltages->cos - dq_voltages->Vq * dq_voltages->sin;
    ab_voltages->Vbeta = dq_voltages->Vq * dq_voltages->cos + dq_voltages->Vd * dq_voltages->sin;
}
