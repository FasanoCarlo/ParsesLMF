/* */

#include "pch.h"
#include <iostream>
#include <regex>
#include <fstream>
#include <windows.h>
#include <ctime>

#define MAXSTILI 5
#define MAXSCRIPT 10
#define FILESERVIZIO "FileDiServizio_LMF_1.txt"
#define DEBUG true
#define NULLO "0_0"

using namespace std;

// Prototipi Funzioni
void scrittaColorata(string, int);
bool scriviFile(string, string);
bool esisteFile(string);
const char* convDaString(string);
bool controlloEspReg(regex, string);
int interpretaBool(string);
string Fdata();
// Funzioni di segnalazione
void imprecisione(string, bool);
void errore(string, bool);
void giusto(string);

// GLOBALI
unsigned int imprecisioni = 0;
bool continua = true;
unsigned int nLinea = 0;

enum const_infoPagina{TIPODOC, CODIFICA, STILE, SCRIPT, TITOLO, TITOLOUNI, DESCRIZIONE, AUTORE, STILELMF, DATA, QTAINFO};
enum caratteri_constchar{SLASH, DOPPIEVIRGOLETTE};


int main(){
	// Messaggio di Benvenuto
    std::cout << "Parser LMF 1.0 >" << Fdata() << "<\n";


	// INPUT nomeFile Entrata e Uscita
	string fileEntrata, fileUscita, linea;
	fstream file;
	
	const char* tempCharAst;
	

	// Espressioni Regolari
	regex versioneLMF("lmf[4-5]");
	regex regFile("(.*?)"); // PROVVISORIO
	regex regFileCSS("(.*?).css"); // PROVVISORIO
	regex regFileJS("(.*?).js"); // PROVVISORIO
	regex regData("^(0?[1-9]|[12][0-9]|3[01])[\/\-](0?[1-9]|1[012])[\/\-]\d{4}$"); // https://stackoverflow.com/questions/15491894/regex-to-validate-date-format-dd-mm-yyyy



	do {
		std::cout << "Percorso file LMF da convertire ('00' per uscire): ";
		cin >> fileEntrata;
		file.open(fileEntrata);
		if (!file.is_open())
			std::cout << "Problemi ad aprire il file. Controlla di aver scritto bene il nome e di avere tutti i permessi necessari\n";
		else
			if (fileEntrata.find(".lmf") == string::npos) {
				// Fonte https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
				imprecisione("Per motivi di ordine ti consigliamo di salavare i file LMF con estennsione '.lmf'", false);
			}
	} while ((!file.is_open()) && fileEntrata != "00");

	do {
		std::cout << "File di uscita: ";
		cin >> fileUscita;
		// Espressione Regolare per controllare che la stringa non contenga caratteri proibiti
		if (!regex_match(fileUscita, regFile))
			std::cout << "I nomi di file non possono contenere i seguenti caratteri: \\ / :  * ?  \" < > | " << endl;
	}while(!regex_match(fileUscita, regFile));


	// Variabili IMPORTANTI
	string infoPagina[QTAINFO];
	int indiceStile = 0, indiceScript = 0;
	bool fineIntroduzione = false;
	int prossimoMettiIn = QTAINFO, nToken;
	bool raccogliParole = false, dichiarazioneAutomatica = true;
	

	// Inizializzazione vettore infoPagina
	for (int i = 0; i < QTAINFO; i++)
		infoPagina[i] = NULLO;
	

	// Interpretazione
	while (getline(file, linea) && continua)
	{
		nLinea++;
		nToken = 0;
		// Ogni Linea
		// Gestione COMMENTI
		const char* caratteri("/\"");
		if (linea.c_str()[0] == caratteri[SLASH] && linea.c_str()[1] == caratteri[SLASH])
			// COMMENTO
			continue;
		else
			if (linea.find("//") != string::npos)
				imprecisione("I Commenti vanno messi in una linea a parte altrimenti vengono contati come testo normale", true);

		string token, valore = "";
		bool esitoScritturaFileServizio = scriviFile(FILESERVIZIO, linea);
		if (esitoScritturaFileServizio) {
			ifstream letturaServizio(FILESERVIZIO);
			while (letturaServizio >> token && continua) {
				// Ogni token
				nToken++;
				dichiarazioneAutomatica = true;
				if(DEBUG)
					std::cout << token << "\n";

				if (raccogliParole) {
					if (token.back() == caratteri[DOPPIEVIRGOLETTE]) {
						// Abbiamo finito di raccogliere il valore togliamo le ultime virgolette
						valore += " ";
						for (int i = 0; i < token.length() - 1; i++)
							valore += token.c_str()[i];
						raccogliParole = false;

						if (prossimoMettiIn == STILE) {
							if(infoPagina[STILE] != NULLO)
								errore("Massimo 1 foglio di stile", true);
							else
								giusto("'" + valore + "' impostato come foglio di stile");
						}

						if (prossimoMettiIn == SCRIPT) {
							if (infoPagina[SCRIPT] != NULLO)
								errore("Massimo 1 file di script", true);
							else
								giusto("'" + valore + "' impostato come file di script");
						}

						if (prossimoMettiIn == AUTORE) {
							if (infoPagina[AUTORE] != NULLO)
								errore("Puoi dichiarare solo una volta l'autore. Se ci sono più autori scrivili tutti nella dichiarazione AUTORE separati da una virgola o cio' che si preferisci", true);
							else
								giusto("'" + valore + "' e' l'autore della pagina");
						}
							

						if (prossimoMettiIn == TITOLO) {
							if (infoPagina[TITOLO] != NULLO)
								errore("Puoi dichiarare solo un titolo", true);
							else
								giusto("'" + valore + "' impostato come titolo");
						}

						if (prossimoMettiIn == DESCRIZIONE) {
							if (infoPagina[DESCRIZIONE] != NULLO)
								errore("Puoi dichiarare solo una descrizione", true);
							else
								giusto("Descrizione dichiarata con successo");
						}


						if (prossimoMettiIn == AUTORE) {
							if (infoPagina[AUTORE] != NULLO)
								errore("Puoi dichiarare solo una volta l'autore. Se ci sono più autori scrivili tutti nella dichiarazione AUTORE separati da una virgola o cio' che si preferisci", true);
							else
								giusto("'" + valore + "' e' l'autore della pagina");
						}

						infoPagina[prossimoMettiIn] = valore;
						prossimoMettiIn = QTAINFO;
					}
					else {
						valore += " " + token;
						continue;
					}
						

				}

				if (!fineIntroduzione) {
					// Siamo ancora nell'introduzione
					if (prossimoMettiIn == QTAINFO) {
						// Nuova Dichiarazione
						bool riconosciuto = false;
						if (token == "TIPODOC") {
							prossimoMettiIn = TIPODOC;
							riconosciuto = true;
						}
						if (token == "CODIFICA") {
							prossimoMettiIn = CODIFICA;
							riconosciuto = true;
						}
						if (token == "STILE") {
							prossimoMettiIn = STILE;
							riconosciuto = true;
						}
						if (token == "SCRIPT") {
							prossimoMettiIn = SCRIPT;
							riconosciuto = true;
						}							
						if (token == "TITOLO") {
							prossimoMettiIn = TITOLO;
							riconosciuto = true;
						}
						if (token == "TITOLOUNI") {
							prossimoMettiIn = TITOLOUNI;
							riconosciuto = true;
						}

						if (token == "DESCRIZIONE") {
							prossimoMettiIn = DESCRIZIONE;
							riconosciuto = true;
						}

						if (token == "AUTORE") {
							prossimoMettiIn = AUTORE;
							riconosciuto = true;
						}

						if (token == "STILELMF") {
							prossimoMettiIn = STILELMF;
							riconosciuto = true;
						}

						if (token == "DATA") {
							prossimoMettiIn = DATA;
							riconosciuto = true;
						}

						if (!riconosciuto && nToken == 1)
							errore("'" + token + "' non e' un token riconosciuto", true);
							
					}
					else {

						// Dovrebbe esserci un valore
						
						if (token.c_str()[0] == caratteri[DOPPIEVIRGOLETTE] && token.c_str()[token.length() - 1] == caratteri[DOPPIEVIRGOLETTE]) {
							// Il valore è una sola parola, non una frase

							// Rimozione Virgolette
							for (int i = 1; i < token.length() - 1; i++)
								valore += token.c_str()[i];

							if (prossimoMettiIn == TIPODOC) {
								// Controllo Per non sovrascrivere
								if (infoPagina[TIPODOC] != NULLO)
									errore("Si puo' dichiarare il tipo documento solo una volta", true);

								if (!controlloEspReg(versioneLMF, valore)) {
									cout << valore << endl;
									errore("Le versioni supportate (nonche' le uniche esistenti) di LMF sono 4 e 5.", true);
								}
								else 
									giusto("Stai elaborando un file LMF versione " + valore);
							}
							if (prossimoMettiIn == CODIFICA) {
								if (valore != "utf8" && valore != "UTF8")
									imprecisione("La codifica consigliata e' UTF8", false);
								if (infoPagina[CODIFICA] != NULLO)
									errore("Si puoì dichiarare la codifica solo una volta", true);
								infoPagina[CODIFICA] = valore;
								giusto("Impostata come codifica: " + valore);
							}
							if (prossimoMettiIn == STILE) {
								if (!regex_match(valore, regFile))
									// Il nome del file non è corretto e contiene caretteri proibiti
									errore("Il nome del file indicato come foglio di stile (" + valore + ") contiene caratteri proibiti per i nomi dei file", false);
								if (!regex_match(valore, regFileCSS))
									// Il nome del file non ha estensione CSS
									imprecisione("Il file di stile non ha estensione .css", true);
								if (infoPagina[STILE] != NULLO && prossimoMettiIn == STILE)
									errore("Massimo 1 foglio di stile", true);
								else
									giusto("'" + valore + "' impostato come foglio di stile");
							}
							if (prossimoMettiIn == SCRIPT) {
								if (!regex_match(valore, regFile))
									// Il nome del file non è corretto e contiene caretteri proibiti
									errore("Il nome del file indicato come file di script (" + valore + ") contiene caratteri proibiti per i nomi dei file", false);
								if (!regex_match(valore, regFileJS))
									// Il nome del file non ha estensione JS
									imprecisione("Il file di stile non ha estensione .js", true);
								if (infoPagina[SCRIPT] != NULLO && prossimoMettiIn == SCRIPT)
									errore("Massimo 1 file di script", true);
								else
									giusto("'" + valore + "' impostato come file di script");
							}
							
							if (prossimoMettiIn == TITOLO) {
								if (infoPagina[TITOLO] != NULLO)
									errore("Puoi dichiarare solo un titolo", true);
								else
									giusto("'" + valore + "' impostato come titolo");
							}
								

							if (prossimoMettiIn == TITOLOUNI) {
								if (infoPagina[TITOLOUNI] != NULLO)
									errore("Si può scegliere se dichiarare o no il titolo universale solo una volta", true);
								if (infoPagina[TITOLO] == NULLO)
									errore("Prima di dichiarare il titolo universale devi dichiarare il titolo", true);
								
								switch (interpretaBool(valore)) {
									case 0:
										infoPagina[TITOLOUNI] = "falso";
										prossimoMettiIn = QTAINFO;
										giusto("Il titolo non e' stato dichiarato universale");
										dichiarazioneAutomatica = false;
										break;
									case 1:
										infoPagina[TITOLOUNI] = "vero";
										prossimoMettiIn = QTAINFO;
										giusto("'" + infoPagina[TITOLO] + "' e' stato impostato come titolo universale e verra' messo nel tag <titolo1></titolo1>");
										dichiarazioneAutomatica = false;
										break;
									case 2:
										errore("'" + valore + "' non e' un valore valido per il token dichiarativo TITOLOUNI", true);
										break;
									default:
										errore("Errore del programma", true);
								}
									
							}
							
							if (prossimoMettiIn == DESCRIZIONE) {
								if (infoPagina[DESCRIZIONE] != NULLO)
										errore("Puoi dichiarare solo una descrizione", true);
								else
									giusto("Descrizione dichiarata con successo");
							}
								

							if (prossimoMettiIn == AUTORE) {
								if (infoPagina[AUTORE] != NULLO)
									errore("Puoi dichiarare solo una volta l'autore. Se ci sono più autori scrivili tutti nella dichiarazione AUTORE separati da una virgola o cio' che si preferisci", true);
								else
									giusto("'" + valore + "' e' l'autore della pagina");
							}

							if (prossimoMettiIn == STILELMF) {
								if (infoPagina[STILELMF] != NULLO)
									errore("Puoi dichiarare solo una volta la scelta di usare o no lo stile LMF", true);
								
								switch (interpretaBool(valore))
								{
									case 0:
										infoPagina[STILELMF] = "falso";
										prossimoMettiIn = QTAINFO;
										giusto("Hai deciso di NON utilizzare lo stile LMF");
										dichiarazioneAutomatica = false;
										break;
									case 1:
										infoPagina[STILELMF] = "vero";
										prossimoMettiIn = QTAINFO;
										giusto("Hai deciso di utilizzare lo stile LMF (*GRASSETTO* , _SOTTOLINEATO_ , 'CORSIVO')");
										dichiarazioneAutomatica = false;
										break;
									case 2:
										errore("'" + valore + "' non e' un valore valido per il token dichiarativo TITOLOUNI", true);
										break;
									default:
										errore("Errore del programma", true);
								}
							}

							if (prossimoMettiIn == DATA) {
								if (infoPagina[DATA] != NULLO)
									errore("Puoi dichiarare la data una sola volta", true);
								if (valore == "auto") {
									// SERVE FUNZIONEDATA
									infoPagina[DATA] = Fdata();
									prossimoMettiIn = QTAINFO;
									dichiarazioneAutomatica = false;
								}
								if (!regex_match(valore, regData))
									errore("Errore nella data", true);
							}
								

							if (dichiarazioneAutomatica) {
								infoPagina[prossimoMettiIn] = valore;
								prossimoMettiIn = QTAINFO;
							}
							

						}
						else {
							// Il valore contiene spazi
							if (prossimoMettiIn == TIPODOC)
								errore("Il tipo documento non puo' contenere spazi", true);
							if (prossimoMettiIn == CODIFICA)
								errore("La codifica non puo' contenere spazi", true);
							if (prossimoMettiIn == STILE)
								imprecisione("Si consiglia di non mettere spazi nei nomi dei file", true);
							if (prossimoMettiIn == SCRIPT)
								imprecisione("Si consiglia di non mettere spazi nei nomi dei file", true);
							if (prossimoMettiIn == TITOLOUNI)
								errore("Gli unici valori che TITOLOUNI puo' assumere sono 'vero', 'falso', 's', 'n', 'S', 'N'. Non puo' contenere spazi", true);
							if (prossimoMettiIn == STILELMF)
								errore("Gli unici valori che STILELMF puo' assumere sono 'vero', 'falso', 's', 'n', 'S', 'N'. Non puo' contenere spazi", true);
							if (prossimoMettiIn == DATA)
								errore("Non sono consentiti spazi nella data. Usa 'auto' per mettere automaticamente la data della compilazione", true);

							// Rimozione virgolette
							valore = "";
							for(int i = 1; i < token.length(); i++)
								valore += token.c_str()[i];
							raccogliParole = true;
						}
								
					}
				}
				else {
					// Corpo della Pagina
				}
				
			}
			letturaServizio.close();
		}
		else
			std::cout << "Errore di scittura. Controlla di avere tutti i permessi necessari per scrivere nella cartella.";
		
	}
	
	file.close();


	if (DEBUG) {
		cout << "\n\n --- DEBUG mostra Vettore infoPagina --- \n";
		cout << "TIPODOC >" << infoPagina[TIPODOC] << "<\n";
		cout << "CODIFICA >" << infoPagina[CODIFICA] << "<\n";
		cout << "STILE >" << infoPagina[STILE] << "<\n";
		cout << "SCRIPT >" << infoPagina[SCRIPT] << "<\n";
		cout << "TITOLO >" << infoPagina[TITOLO] << "<\n";
		cout << "TITOLOUNI >" << infoPagina[TITOLOUNI] << "<\n";
		cout << "DESCRIZIONE >" << infoPagina[DESCRIZIONE] << "<\n";
		cout << "AUTORE >" << infoPagina[AUTORE] << "<\n";
		cout << "STILELMF >" << infoPagina[STILELMF] << "<\n";
		cout << "DATA >" << infoPagina[DATA] << "<\n";
	}

	return 0;
}


void scrittaColorata(string testo, int colore) {
	// Fonte: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal

	//                       ROSSO 0      VERDE 1       GIALLO 2      CIANO 3     MAGENTA 4
	string colori[5] = { "\033[0;31m", "\033[1;32m", "\033[1;33m", "\033[0;36m", "\033[0;35m" };
	const string reset("\033[0m");

	std::cout << colori[colore] << testo << reset << "\n";

	return;
}

bool scriviFile(string nomeFile, string testo) {
	try {
		ofstream file;
		file.open(nomeFile);
		file << testo;
		file.close();
		return true;
	}
	catch(exception e){
		return false;
	}
}

bool esisteFile(string nomeFile) {
	ifstream file(nomeFile);
	if (file)
		return true;
	else
		return false;
}

const char* convDaString(string Stringa){
	const char* costante_char = Stringa.c_str();
	return costante_char;
}

void imprecisione(string stringaImprecisione, bool incrementa) {
	string messaggio("IMPRECISIONE (Linea " + to_string(nLinea) + "): ");
	messaggio.append(stringaImprecisione);
	scrittaColorata(messaggio, 2);
	if (incrementa)
		imprecisioni++;
}


void errore(string stringaErrore, bool ferma) {
	string messaggio("ERRORE (Linea " + to_string(nLinea) + "): ");
	messaggio.append(stringaErrore);
	scrittaColorata(messaggio, 0);
	if (ferma) {
		continua = false;
		cout << "\n\n";
		system("pause");
		exit(0);
	}
		
}

void giusto(string stringa) {
	string messaggio("CORRETTO: ");
	messaggio.append(stringa);
	scrittaColorata(messaggio, 1);
}

bool controlloEspReg(regex espressioneRegolare, string stringa) {
	if (regex_match(stringa, espressioneRegolare))
		return true;
	else
		return false;
}

int interpretaBool(string valore) {
	if (valore == "vero" || valore == "S" || valore == "s")
		return 1; // Vero
	else
		if (valore == "falso" || valore == "N" || valore == "n")
			return 0; // Falso
		else
			return 2; // Errore
}

string Fdata() {
	time_t adesso = time(0);
	struct tm adessoLocale;
	string data;

	adesso = time(NULL);
	localtime_s(&adessoLocale, &adesso);

	data += to_string(adessoLocale.tm_mday) + "/";
	data += to_string(adessoLocale.tm_mon + 1) + "/";
	data += to_string(adessoLocale.tm_year + 1900);

	return data;
}