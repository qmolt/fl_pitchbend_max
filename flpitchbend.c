#include "flpitchbend.h"

void C74_EXPORT main() {

	t_class *c;

	c = class_new("flpitchbend",(method)fl_pitchbend_new, (method)NULL,sizeof(t_fl_pitchbend), 0, 0);
	class_addmethod(c, (method)fl_pitchbend_assist,"assist", A_CANT, 0);
	class_addmethod(c, (method)fl_pitchbend_int, "int", A_LONG, 0);
	class_addmethod(c, (method)fl_pitchbend_float, "float", A_FLOAT, 0);

	class_addmethod(c, (method)fl_pitchbend_list, "list", A_GIMME, 0);

	class_addmethod(c, (method)fl_pitchbend_tunei, "tunei", A_GIMME, 0);
	class_addmethod(c, (method)fl_pitchbend_tunef, "tunef", A_GIMME, 0);
	class_addmethod(c, (method)fl_pitchbend_div_oct, "div_oct", A_GIMME, 0);

	class_register(CLASS_BOX, c);
	fl_pitchbend_class = c; 
}

void *fl_pitchbend_new(t_symbol *s, short argc, t_atom *argv) 
{
	t_fl_pitchbend *x = (t_fl_pitchbend *)object_alloc(fl_pitchbend_class);

	x->m_outlet3 = intout((t_object *)x);
	x->m_outlet2 = intout((t_object *)x);
	x->m_outlet1 = intout((t_object *)x);

	x->f0_i = FREC0_STDR;
	x->f0_f = FREC0_STDR;
	x->n_i = DIV_OCT_STDR;
	//x->n_f = DIV_OCT_STDR //12
	x->m0_i = MIDI0_STDR;
	x->m0_f = MIDI0_STDR;

	return x;
}

void fl_pitchbend_assist(t_fl_pitchbend *x, void *b, long msg, long arg, char *dst)
{
	if (msg == ASSIST_INLET) {										
		switch (arg) {
		case I_INPUT: 
			sprintf(dst, "midi note"); 
			break;
		}
	}
	else if (msg == ASSIST_OUTLET) {    
		switch (arg) {
		case O_MIDIOUT: 
			sprintf(dst, "midi note"); 
			break;
		case O_MSB:
			sprintf(dst, "MSB");
			break;
		case O_LSB:
			sprintf(dst, "LSB");
			break;
		}
	}
}

void fl_pitchbend_int(t_fl_pitchbend *x, long n) 
{
	fl_pitchbend_float(x, (double)n);
}

void fl_pitchbend_float(t_fl_pitchbend *x, double f) 
{
	double m_i = f;
	double f0_i = x->f0_i;
	double f0_f = x->f0_f;
	double m0_i = x->m0_i;
	double m0_f = x->m0_f;
	double n_i = x->n_i;

	double m_f = 12 * log2(f0_i / x->f0_f * pow(2.0, (m_i - m0_i) / n_i)) + m0_f;
	double im_f = floor(m_f);

	long pb = 8192 + (long)(4095.0 * (m_f - im_f));
	long msb = pb >> 7;
	long lsb = pb & 127;

	outlet_int(x->m_outlet3, lsb);
	outlet_int(x->m_outlet2, msb);
	outlet_int(x->m_outlet1, (long)im_f);
}

void fl_pitchbend_list(t_fl_pitchbend *x, t_symbol *s, long argc, t_atom *argv) 
{
	long ac = argc;
	t_atom *ap = argv;

	float m_i;
	float wheel;

	if (ac != 2) { object_error((t_object *)x, "list [2 args]: midi note, pitch bend"); return; }

	m_i = (float)atom_getfloat(ap); 
	wheel = (float)atom_getfloat(ap + 1);
	wheel = MIN(MAX(wheel, 0.f), 127.f);

	m_i = (float)floor(m_i) + (wheel/63.5f);

	fl_pitchbend_float(x, (double)m_i);
}

void fl_pitchbend_tunei(t_fl_pitchbend *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;
	double f0_i;
	double m0_i;

	if (ac != 2) { object_error((t_object *)x, "tunei [2 args]: reference freq, reference midi note"); return; }
	
	if (atom_gettype(ap) != A_LONG && atom_gettype(ap) != A_FLOAT) { object_error((t_object *)x, "tunei: reference freq must be a number"); return; }
	if (atom_gettype(ap + 1) != A_LONG && atom_gettype(ap + 1) != A_FLOAT) { object_error((t_object *)x, "tunei: reference midi note must be a number"); return; }
	
	f0_i = (double)atom_getfloat(ap);
	m0_i = (double)atom_getfloat(ap + 1);
	
	if (f0_i < 1.0) { object_error((t_object *)x, "tunei: freq must be greather than 1"); return; }

	x->f0_i = f0_i;
	x->m0_i = m0_i;
}

void fl_pitchbend_tunef(t_fl_pitchbend *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;
	double f0_f;
	double m0_f;

	if (ac != 2) { object_error((t_object *)x, "tunef [2 args]: reference freq, reference midi note"); return; }

	if (atom_gettype(ap) != A_LONG && atom_gettype(ap) != A_FLOAT) { object_error((t_object *)x, "tunef: reference freq must be a number"); return; }
	if (atom_gettype(ap + 1) != A_LONG && atom_gettype(ap + 1) != A_FLOAT) { object_error((t_object *)x, "tunef: reference midi note must be a number"); return; }

	f0_f = (double)atom_getfloat(ap);
	m0_f = (double)atom_getfloat(ap + 1);

	if (f0_f < 1.0) { object_error((t_object *)x, "tunef: freq must be greather than 1"); return; }

	x->f0_f = f0_f;
	x->m0_f = m0_f;
}

void fl_pitchbend_div_oct(t_fl_pitchbend *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;

	double n_i;

	if (ac != 1) { object_error((t_object *)x, "div_oct [1 arg]: octave division"); return; }

	if (atom_gettype(ap) != A_LONG && atom_gettype(ap) != A_FLOAT) { object_error((t_object *)x, "div_oct: division must be a number"); return; }
	
	n_i = (float)atom_getfloat(ap);

	if (n_i < 1.0) { object_error((t_object *)x, "div_oct: division must be greater than 1"); return; }
	
	x->n_i = n_i;
}
