#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

map<string, string> registros = {
    {"$zero", "00000"}, {"$at", "00001"}, {"$v0", "00010"}, {"$v1", "00011"},
    {"$a0", "00100"}, {"$a1", "00101"}, {"$a2", "00110"}, {"$a3", "00111"},
    {"$t0", "01000"}, {"$t1", "01001"}, {"$t2", "01010"}, {"$t3", "01011"},
    {"$t4", "01100"}, {"$t5", "01101"}, {"$t6", "01110"}, {"$t7", "01111"},
    {"$s0", "10000"}, {"$s1", "10001"}, {"$s2", "10010"}, {"$s3", "10011"},
    {"$s4", "10100"}, {"$s5", "10101"}, {"$s6", "10110"}, {"$s7", "10111"},
    {"$t8", "11000"}, {"$t9", "11001"}, {"$k0", "11010"}, {"$k1", "11011"},
    {"$gp", "11100"}, {"$sp", "11101"}, {"$fp", "11110"}, {"$ra", "11111"}
};

map<string, pair<string, string>> instrucciones = {
    {"add", {"000000", "100000"}}, {"addu", {"000000", "100001"}},
    {"sub", {"000000", "100010"}}, {"subu", {"000000", "100011"}},
    {"and", {"000000", "100100"}}, {"or", {"000000", "100101"}},
    {"xor", {"000000", "100110"}}, {"nor", {"000000", "100111"}},
    {"slt", {"000000", "101010"}}, {"sltu", {"000000", "101011"}},
    {"sll", {"000000", "000000"}}, {"srl", {"000000", "000010"}},
    {"sra", {"000000", "000011"}}, {"jr", {"000000", "001000"}},
    {"addi", {"001000", ""}}, {"addiu", {"001001", ""}},
    {"andi", {"001100", ""}}, {"ori", {"001101", ""}},
    {"xori", {"001110", ""}}, {"lui", {"001111", ""}},
    {"lw", {"100011", ""}}, {"sw", {"101011", ""}},
    {"lb", {"100000", ""}}, {"lbu", {"100100", ""}},
    {"sb", {"101000", ""}}, {"beq", {"000100", ""}},
    {"bne", {"000101", ""}}, {"slti", {"001010", ""}},
    {"sltiu", {"001011", ""}}, {"j", {"000010", ""}},
    {"jal", {"000011", ""}}
};




int calcularDesplazamiento(unsigned int direccionEtiqueta, unsigned int pc){
    int offset = (direccionEtiqueta - (pc + 4)) / 4;
    return offset;
}

//para eliminar todos los comentarios o lineas vacias
string limpiarLinea(string linea) {
    int posComentario = linea.find('#');
    if (posComentario != string::npos){
        linea = linea.substr(0, posComentario);
    }
    //eliminamos espacios y tabs al inicio de la linea
    int inicio = 0;
    while (inicio < linea.length() && (linea[inicio] == ' ' || linea[inicio] == '\t')){
        inicio++;
    }
    linea = linea.substr(inicio);
    //eliminamos espacios y tabs al final de la linea

    int fin = linea.length();
    while (fin > 0 && (linea[fin - 1] == ' ' || linea[fin - 1] == '\t')){
        fin--;
    }

    linea = linea.substr(0, fin);

    return linea;
}
// FUE SOLO UNA IDEA AUN NO SE SI SIRVE
string complementoADos(int valor, int bits = 16) { //ajustar bits 
    string resultado;

    if (valor < 0) {
        valor = (1 << bits) + valor;  // Convierte el número negativo a complemento a dos
    }

    for (int i = bits - 1; i >= 0; i--) {
        resultado += to_string((valor >> i) & 1); // to_string pasa directamente los bits a caracteres
    }

    return resultado;
} 

vector<string> traducir(vector<string> ins){
    vector<string> resultado;
    int i = 0;
    while(i < ins.size()){

        string nombre = ins[i]; // Nombre de la instruccion

        if (instrucciones.find(nombre) != instrucciones.end()) {
            string opcode = instrucciones[nombre].first; // Opcode de la instrucción
            string funct = instrucciones[nombre].second; // Funct solo para tipo r

            if (opcode == "000000") { // Instruccion tipo R
                string rs = registros[ins[i + 1]]; 
                string rt = registros[ins[i + 2]]; 
                string rd = registros[ins[i + 3]]; 
                string shamt = ins[i + 4]; 

                string binario = opcode + rs + rt + rd + shamt + funct;
                resultado.push_back(binario);

                i += 5; // Avanzar al siguiente grupo de inss
            } else if (nombre == "j" || nombre == "jal") { // Instruccion tipo J
                string direccion = ins[i + 1]; 
                
                string binario = opcode + direccion;
                resultado.push_back(binario);

                i += 2; 
            } else { // instruccion tipo I
                string rs = registros[ins[i + 1]]; 
                string rt = registros[ins[i + 2]]; 
                string inmediato = ins[i + 3]; 

                string binario = opcode + rs + rt + inmediato;
                resultado.push_back(binario);

                i += 4; 
            }
        } else {
            cerr << "Error: Instruccion no reconocida: " << nombre << endl;
            i++; 
        }
    }
    return resultado;
}

bool esRegistroValido(const string &registro) {
    return registros.find(registro) != registros.end();
}


bool validarTipoR(string &instruccion, vector<string> &ins) {
    bool ans = false;
    istringstream ss(instruccion);
    string nombre;
    ss >> nombre;

    // Reemplazar comas por espacios en la instrucción
    for (int i = 0; i < instruccion.length();i++) {
        if (instruccion[i] == ',') {
            instruccion[i] = ' ';
        }
    }

    // Volver a crear el stringstream con la instrucción modificada
    ss.clear();
    ss.str(instruccion);
    ss >> nombre;

    if (nombre == "mult" || nombre == "multu" || nombre == "div" || nombre == "divu") {
        string rs, rt;
        ss >> rs >> rt;
        ans = esRegistroValido(rs) && esRegistroValido(rt);
        if (ans) {
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back(rt);
            ins.push_back("00000");
            ins.push_back("00000");
        }
    } else if (nombre == "jr" || nombre == "mfhi" || nombre == "mflo") {
        string rs;
        ss >> rs;
        ans = esRegistroValido(rs);
        if (ans) {
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back("00000");
            ins.push_back("00000");
            ins.push_back("00000");
        }
    } else if (nombre == "mfc0") {
        string rd, rs;
        ss >> rd >> rs;
        ans = esRegistroValido(rd) && esRegistroValido(rs);
        if (ans) {
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back("00000");
            ins.push_back(rd);
            ins.push_back("00000");
        }
    } else if (nombre == "sll" || nombre == "srl" || nombre == "sra") {
        string rd, rt, shamt;
        ss >> rd >> rt >> shamt;
        ans = esRegistroValido(rd) && esRegistroValido(rt);
        if (ans) {
            ins.push_back(nombre);
            ins.push_back("00000");
            ins.push_back(rt);
            ins.push_back(rd);
            ins.push_back(shamt);
        }
    } else if (nombre == "add" || nombre == "addu" || nombre == "sub" || nombre == "subu" ||
               nombre == "and" || nombre == "or" || nombre == "nor" || nombre == "slt" || nombre == "sltu") {
        string rd, rs, rt;
        ss >> rd >> rs >> rt;

        //cout << rd << endl << rs << endl << rt;
        ans = esRegistroValido(rd) && esRegistroValido(rs) && esRegistroValido(rt);
        if (ans) {
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back(rt);
            ins.push_back(rd);
            ins.push_back("00000");
        }
    }
    return ans;
}

bool validarTipoI(string &instruccion, vector<string> &ins, map<string, unsigned int> &etiquetas, unsigned int pc) {
    bool ans = false;
    istringstream ss(instruccion);
    string nombre;
    ss >> nombre;

    // Reemplazar comas por espacios en la instrucción
    for (int i = 0; i < instruccion.length();i++) {
        if (instruccion[i] == ',') {
            instruccion[i] = ' ';
        }
    }

    // Volver a crear el stringstream con la instrucción modificada
    ss.clear();
    ss.str(instruccion);
    ss >> nombre;

    if (nombre == "beq" || nombre == "bne") {
        // Formato: rs rt etiqueta
        string rs, rt, etiqueta;
        ss >> rs >> rt >> etiqueta;

        // Validar registros y etiqueta
        ans = esRegistroValido(rs) && esRegistroValido(rt) && (etiquetas.find(etiqueta) != etiquetas.end());

        if (ans) {
            int offset = calcularDesplazamiento(etiquetas[etiqueta], pc);
            string offsetBinario = complementoADos(offset);
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back(rt);
            ins.push_back(offsetBinario);
            
        } else {
            cerr << "Error: Registros o etiqueta no válidos en la instrucción: " << instruccion << endl;
        }
    } else if (nombre == "addi" || nombre == "addiu" || nombre == "andi" || nombre == "ori" || nombre == "slti" || nombre == "sltiu") {
        // Formato: rt rs inmediato
        string rt, rs, inmediato;
        ss >> rt >> rs >> inmediato;

        // Validar registros
        ans = esRegistroValido(rt) && esRegistroValido(rs);

        if (ans) {
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back(rt);
            ins.push_back(inmediato);
        } else {
            cerr << "Error: Registros no válidos en la instrucción: " << instruccion << endl;
        }
    } else if (nombre == "lw" || nombre == "sw" || nombre == "lb" || nombre == "lbu" || nombre == "sb" || nombre == "ll" || nombre == "sc") {
        // Formato: rt offset(rs)
        string rt, rs;
        ss >> rt >> rs;

        // Extraer offset y registro base
        size_t parenAbierto = rs.find('(');
        size_t parenCerrado = rs.find(')');
        if (parenAbierto == string::npos || parenCerrado == string::npos) {
            cerr << "Error: Formato incorrecto en la instrucción: " << instruccion << endl;
            ans = false;
        } else {
            string offset = rs.substr(0, parenAbierto);
            string reg = rs.substr(parenAbierto + 1, parenCerrado - parenAbierto - 1);

            // Validar registros
            ans = esRegistroValido(rt) && esRegistroValido(reg);

            if (ans) {
                ins.push_back(nombre);
                ins.push_back(reg);
                ins.push_back(rt);
                ins.push_back(offset);
            } else {
                cerr << "Error: Registros no válidos en la instrucción: " << instruccion << endl;
            }
        }
    } else if (nombre == "lui") {
        // Formato: rt inmediato
        string rt, inmediato;
        ss >> rt >> inmediato;

        // Validar registro
        ans = esRegistroValido(rt);

        if (ans) {
            ins.push_back(nombre);
            ins.push_back("00000");
            ins.push_back(rt);
            ins.push_back(inmediato);

        } else {
            cerr << "Error: Registro no válido en la instrucción: " << instruccion << endl;
        }
    } else {
        cerr << "Error: Instrucción no reconocida: " << instruccion << endl;
        ans = false;
    }

    return ans;
}

bool validarTipoJ(string &instruccion, vector<string> &ins, map<string, unsigned int> &etiquetas) {
    bool ans = false;
    istringstream ss(instruccion);
    string nombre, etiqueta;

    ss >> nombre >> etiqueta;

    if (etiqueta.empty()) {
        cerr << "Error: La instruccion tipo J: " << instruccion << " esta mal estructurada" << endl;
        ans = false;
    } else {
        if (etiquetas.find(etiqueta) == etiquetas.end()) {
            cerr << "Error: Etiqueta no encontrada: " << etiqueta << endl;
            ans = false;
        } else {
            if (ans) {
                ins.push_back(nombre);
                ins.push_back(to_string(etiquetas[etiqueta]));
            }
        }
    }
    return ans;
}

void leerEtiquetas(string nombre, map<string, unsigned int> &etiquetas, unsigned int &pc){
    ifstream archivo(nombre);
    string linea;
    unsigned int contadorLinea = 0;

    if (!archivo.is_open()){
        cerr << "Error! No se pudo abrir el archivo " << nombre << endl;
    }else{
        while (getline(archivo, linea)) {
            string lineaLeida = limpiarLinea(linea);
            if (!lineaLeida.empty()) {
                if (lineaLeida.back() == ':'){
                    string etiqueta = lineaLeida.substr(0, lineaLeida.length() - 1);
                    etiquetas[etiqueta] = pc + (contadorLinea * 4);
                }
                contadorLinea++;
            }
        }
    }

    archivo.close();
}

void leerTxt(string nombre, vector<string> &ins, unsigned int &pc, map<string, unsigned int> &etiquetas){
    ifstream archivo(nombre);
    int countLinea = 0;

    if(!archivo.is_open()){
        cerr << "Error! No se pudo abrir el archivo: "<< nombre <<endl;
        archivo.close();
    }else{
        leerEtiquetas(nombre, etiquetas, pc);
        string linea;
        while(getline(archivo, linea)){
            //cout << linea << endl;
            //cout << "jfkds";
            //cout << endl;
            string lineaLeida = limpiarLinea(linea);
            //cout << lineaLeida<< endl;
            if (!lineaLeida.empty()) {
                if (lineaLeida.back() != ':'){ // Ignorar líneas de etiquetas
                    if (validarTipoR(lineaLeida, ins)){
                        pc += 4;
                    } else if (validarTipoJ(lineaLeida, ins, etiquetas)){
                        //cout << "holaa";
                        pc += 4;
                    } else if (validarTipoI(lineaLeida, ins, etiquetas, pc)){
                        pc += 4;
                    } else {
                        cerr << "Error: Instruccion no reconocida: " << lineaLeida << endl;
                    }
                }
            }else{
                
            }
        }
    }

    if(archivo.eof()){
        archivo.close();
    }
}

void generarTxt(vector<string> &traduccion, string &nombreArchivo){
    ofstream archivo(nombreArchivo);

    if (!archivo.is_open()){
        cerr << "Error: No se pudo crear el archivo :(" << nombreArchivo << endl;
        return;
    }

    for (const string &linea : traduccion){
        archivo << linea << endl; 
    }

    archivo.close();
    cout << "Traduccion guardada en: " << nombreArchivo << endl;
}

int main(){
    string nombre;
    unsigned int pc;
    cout << "Ingrese el nombre del archivo que desea traducir: ";
    cin >> nombre ;
    cout << endl << "Ingrese la direccion de memoria donde comienza su programa: ";
    cin >> pc;

    map<string, unsigned int> etiquetas;
    vector<string> instrucciones;
    leerTxt(nombre, instrucciones,pc, etiquetas);

    for(int i = 0; i < instrucciones.size(); i++){
        cout << instrucciones[i] << " - " ;
    }

    cout << endl;

    vector<string> traduccion = traducir(instrucciones);

    // Guardar la traducción en un archivo de texto
    string nombreArchivoSalida = "traduccionMIPS.txt";
    generarTxt(traduccion, nombreArchivoSalida);

    return 0;
}