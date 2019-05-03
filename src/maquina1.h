#ifndef MAQUINA1_H
#define MAQUINA1_H

#include "icones/next.h"

#include "icones/diario.h"
#include "icones/fast.h"
#include "icones/centrifuge.h"
#include "icones/pesado.h"
#include "icones/enxague.h"
#include "tipos.h"


t_ciclo c_rapido = {.nome = "Rapido",
                    .enxagueTempo = 5,
                    .enxagueQnt = 3,
                    .centrifugacaoRPM = 900,
                    .centrifugacaoTempo = 5,
                    .heavy = 0,
                    .bubblesOn = 1,
					.image = &fast
                  };

t_ciclo c_diario = {.nome = "Diario",
                    .enxagueTempo = 15,
                    .enxagueQnt = 2,
                    .centrifugacaoRPM = 1200,
                    .centrifugacaoTempo = 8,
                    .heavy = 0,
                    .bubblesOn = 1,
					.image = &diario
                  };

t_ciclo c_pesado = {.nome = "Pesado",
                     .enxagueTempo = 10,
                     .enxagueQnt = 3,
                     .centrifugacaoRPM = 1200,
                     .centrifugacaoTempo = 10,
                     .heavy = 1,
                     .bubblesOn = 1,
					 .image = &pesado
                  };

t_ciclo c_enxague = {.nome = "Enxague",
                     .enxagueTempo = 10,
                     .enxagueQnt = 1,
                     .centrifugacaoRPM = 0,
                     .centrifugacaoTempo = 0,
                     .heavy = 0,
                     .bubblesOn = 0,
					 .image = &enxague
                  };

t_ciclo c_centrifuga = {.nome = "Centrifuga",
                     .enxagueTempo = 0,
                     .enxagueQnt = 0,
                     .centrifugacaoRPM = 1200,
                     .centrifugacaoTempo = 10,
                     .heavy = 0,
                     .bubblesOn = 0,
					 .image = &centrifuge
};


t_ciclo *initMenuOrder();

#endif
