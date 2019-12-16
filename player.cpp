#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar



// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Valoracion(const Environment &estado, int jugador){
    int juntas, o_juntas;
    juntas = o_juntas = 0;
    int puntuacion = 0;

    int o_jugador = 1;
    if(jugador == 1){
        o_jugador = 2;
    }

    for(int i=0; i<7; i++){
        for(int j=0; j<7 ; j++){
            if(estado.See_Casilla(i,j)==jugador){
                for(int k=j ; k<7; k++){
                    if(estado.See_Casilla(i,k)==jugador)
                        juntas++;
                    else
                        break;
                }
                for(int k=i ; k<7; k++){
                    if(estado.See_Casilla(k,j)==jugador)
                        juntas++;
                    else
                        break;
                }
                int l = j;
                for(int k=i ; k<7; k++){
                    if(l>7)
                        break;
                    if(estado.See_Casilla(k,l)==jugador)
                        juntas++;
                    else
                        break;
                    l++;
                }
                l = j;
                for(int k=i ; k>0; k--){
                    if(l<0)
                        break;
                    if(estado.See_Casilla(k,l)==jugador)
                        juntas++;
                    else
                        break;
                    l--;
                }
            }else if(estado.See_Casilla(i,j)==o_jugador){
                for(int k=j ; k<7; k++){
                    if(estado.See_Casilla(i,k)==o_jugador)
                        o_juntas++;
                    else
                        break;
                }
                for(int k=i ; k<7; k++){
                    if(estado.See_Casilla(k,j)==o_jugador)
                        o_juntas++;
                    else
                        break;
                }
                int l = j;
                for(int k=i ; k<7; k++){
                    if(l>7)
                        break;
                    if(estado.See_Casilla(k,l)==o_jugador)
                        o_juntas++;
                    else
                        break;
                    l++;
                }
                l = j;
                for(int k=i ; k>0; k--){
                    if(l<0)
                        break;
                    if(estado.See_Casilla(k,l)==o_jugador)
                        o_juntas++;
                    else
                        break;
                    l--;
                }
            }
            puntuacion += - juntas*1000 + o_juntas*100;
            juntas=0, o_juntas = 0;
        }
    }
    return puntuacion;
}
double Player::Poda_AlfaBeta(Environment estado, int jugador, int inicio, Environment::ActionType & accion, double alpha, double beta, int profundidad){

	Environment::ActionType aux;
		double eval;
		int j = -1;
		Environment hijo;

		bool *VectAct = new bool[8];
		int numAcciones = estado.possible_actions(VectAct);
        bool podo = false;

	if (inicio == profundidad || estado.JuegoTerminado()){
		return Valoracion(estado, jugador);
	}else{
		if (inicio % 2 == 0){
			double maxpuntuacion = menosinf;

			for (int i = 0; i < numAcciones && !podo; i++){
				hijo = estado.GenerateNextMove(j);
				eval = Poda_AlfaBeta(hijo, jugador, inicio + 1, aux, alpha, beta, profundidad);
				if (eval > maxpuntuacion){
					maxpuntuacion = eval;
					accion = (Environment::ActionType) j;
				}
                if (eval > alpha){
					alpha = eval;
				}
				if (beta <= alpha){
					podo=true;
				}
			}
			return maxpuntuacion;
		}else{
			double minpuntuacion = masinf;
            for (int i = 0; i < numAcciones && !podo; i++){
				hijo = estado.GenerateNextMove(j);
				eval = Poda_AlfaBeta(hijo, jugador, inicio + 1, aux, alpha, beta, profundidad);
				if (eval < minpuntuacion){
					minpuntuacion = eval;
					accion = (Environment::ActionType) j;
				}
				if (eval < beta){
					beta = eval;
				}
				if (beta <= alpha){
					podo = true;
				}
			}
			return minpuntuacion;
		}
	}
}




// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}






// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acci�n que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha, beta; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;


    //--------------------- COMENTAR Desde aqui
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }

    //--------------------- COMENTAR Hasta aqui


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    //Opcion: Poda AlfaBeta
    //NOTA: La parametrizacion es solo orientativa
    valor = Poda_AlfaBeta(actual_, jugador_, 0, accion, alpha, beta, PROFUNDIDAD_ALFABETA);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    return accion;
}

