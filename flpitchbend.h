#include "ext.h"
#include "ext_obex.h"
#include <math.h>

//#define FREC0_STDR 261.625565300598
//#define MIDI0_STDR 60.0
#define FREC0_STDR 440.0
#define MIDI0_STDR 69.0
#define DIV_OCT_STDR 12.0
#define MULT_OCT_STDR 2.0

enum INLETS { I_INPUT, NUM_INLETS };
enum OUTLETS { O_MIDIOUT, O_MSB, O_LSB, NUM_OUTLETS };

typedef struct {

	t_object obj;

	double f0_i;
	double f0_f;
	double n_i;
	//double n_f; //12
	double m0_i;
	double m0_f;

	void *m_outlet1;
	void *m_outlet2;
	void *m_outlet3;

} t_fl_pitchbend;

void *fl_pitchbend_new(t_symbol *s, short argc, t_atom *argv);
void fl_pitchbend_assist(t_fl_pitchbend *x, void *b, long msg, long arg, char *dst);

void fl_pitchbend_int(t_fl_pitchbend *x, long n);
void fl_pitchbend_float(t_fl_pitchbend *x, double f); 
void fl_pitchbend_list(t_fl_pitchbend *x, t_symbol *s, long argc, t_atom *argv);

void fl_pitchbend_tunei(t_fl_pitchbend *x, t_symbol *s, long argc, t_atom *argv);
void fl_pitchbend_tunef(t_fl_pitchbend *x, t_symbol *s, long argc, t_atom *argv);
void fl_pitchbend_div_oct(t_fl_pitchbend *x, t_symbol *s, long argc, t_atom *argv);

static t_class *fl_pitchbend_class;