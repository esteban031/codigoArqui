/* 

Parte práctica parcial 1
Traductor MIPS - Binario
Integrantes: Valentina Feijoo y Esteban Arismendi 

*/



#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

/* list<string> tipoR = {"add", "and", "addu", "subu", "mfc0", "mult",
"multu" , "div", "divu", "jr" , "nor", "or", "slt", "sltu", "sll", "srl", "sub", "mfhi", "mflo", "sra"};

list<string> tipoI = {"addi", "addiu", "andi", "beq", "bne", "lbu", "ll", "lui", "lw", "ori",
"slti", "sltiu", "sb", "sc", "sw"};

list<string> tipoJ = {"j", "jal"}; */

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

bool esRegistroValido(string &registro) {
    return find(registros.begin(), registros.end(), registro) != registros.end();
}


bool validarTipoR(string &instruccion, vector<string> &ins){
    bool ans = true;
    istringstream ss(instruccion);
    string nombre, rd, rs, rt, shamt;
    char coma1, coma2, coma3;

    ss >> nombre;

    if(nombre == "add" || nombre == "addu" || nombre == "sub" || nombre == "subu" ||
        nombre == "and" || nombre == "or" || nombre == "nor" || nombre == "slt" || nombre == "sltu"){ //SE COMENTO LO SE SS DE BAAJO
        // Formato: rd, rs, rt
        //ss >> rd >> coma1 >> rs >> coma2 >> rt;

        // Extraer los registros como strings completos
        getline(ss, rd, ',');  // ESTA PARTE FUE AÑADIDA HACIENDO DEBUG
        getline(ss, rs, ',');  
        getline(ss, rt, ',');  

        // DEBUG
        rd = limpiarLinea(rd);
        rs = limpiarLinea(rs);
        rt = limpiarLinea(rt);

        cout << "DEBUG : ";
        cout << rd << rs << rt << endl; 

        ans = esRegistroValido(rd) && esRegistroValido(rs) && esRegistroValido(rt);
        
        if (ans){
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back(rt);
            ins.push_back(rd);
            ins.push_back("00000");

        }else{
            cerr << "Error: La instruccion tipo R: " << instruccion << " esta mal estructurada" <<endl;
        }

    }else if (nombre == "mult" || nombre == "multu" || nombre == "div" || nombre == "divu"){
        // Formato: rs, rt
        //ss >> rs >> coma1 >> rt;

        getline(ss, rd, ',');  // ESTA PARTE FUE AÑADIDA HACIENDO DEBUG
        getline(ss, rs, ','); 
        if (coma1 != ',' || rs[0] != '$' || rt[0] != '$') {
            cerr << "Error: La instruccion tipo R: " << instruccion << " esta mal estructurada" <<endl;
            ans = false;
        }else{
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back(rt);
            ins.push_back("00000");
            ins.push_back("00000");
        }
    }else if (nombre == "jr" || nombre == "mfhi" || nombre == "mflo"){
        // Formato: rs (o rd para mfhi/mflo)
        ss >> rs;
        if (rs[0] != '$') {
            cerr << "Error: La instruccion tipo R: " << instruccion << " esta mal estructurada" <<endl;
            ans = false;
        }else{
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back("00000");
            ins.push_back("00000");
            ins.push_back("00000");
        }
    }else if (nombre == "mfc0"){
        // Formato: rd, rs
        ss >> rd >> coma1 >> rs;
        if (coma1 != ',' || rd[0] != '$' || rs[0] != '$'){
            cerr << "Error: La instruccion tipo R: " << instruccion << " esta mal estructurada" <<endl;
            ans = false;
        }else{
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back("00000");
            ins.push_back(rd);
            ins.push_back("00000");
            
        }
    }else if (nombre == "sll" || nombre == "srl" || nombre == "sra"){
        // Formato: rd, rt, shamt
        ss >> rd >> coma1 >> rt >> coma2 >> shamt;
        if (coma1 != ',' || coma2 != ',' || rd[0] != '$' || rt[0] != '$'){
            cerr<< "Error: La instruccion tipo R: " << instruccion << " esta mal estructurada" <<endl;
            ans = false;
        }else{
            ins.push_back(nombre);
            ins.push_back("00000");
            ins.push_back(rt);
            ins.push_back(rd);
            ins.push_back(shamt);
        }
    }else{
        ans = false;
    }

    return ans;
}

bool validarTipoI(string &instruccion, vector<string> &ins, map<string, unsigned int> &etiquetas, unsigned int pc){
    bool ans = true;
    istringstream ss(instruccion);
    string nombre, rt, rs, inmediato, etiqueta;
    char coma1, coma2;

    ss >> nombre;

    if (nombre == "beq" || nombre == "bne") {
        // Formato: rs, rt, etiqueta
        ss >> rs >> coma1 >> rt >> coma2 >> etiqueta;
        if (coma1 != ',' || coma2 != ',' || rs[0] != '$' || rt[0] != '$') {
            cerr << "Error: La instruccion tipo I: " << instruccion << " esta mal estructurada" << endl;
            ans =  false;
        }else{

            /* if (etiquetas.find(etiqueta) == etiquetas.end()) {
                cerr << "Error: Etiqueta no encontrada en la instrucción tipo I: " << instruccion << endl;
                return false;
            } */

            // Calcular el desplazamiento
            int offset = calcularDesplazamiento(etiquetas[etiqueta], pc);

            // Convertir el desplazamiento a complemento a dos (si es negativo) no sirve jeje
            string offsetBinario = complementoADos(offset);

            // Añadir la instrucción al vector con el desplazamiento en binario
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back(rt);
            ins.push_back(offsetBinario); // Desplazamiento en binario
        }
    } else if (nombre == "addi" || nombre == "addiu" || nombre == "andi" || nombre == "ori" || nombre == "slti" || nombre == "sltiu"){
        // Formato: rt, rs, inmediato
        ss >> rt >> coma1 >> rs >> coma2 >> inmediato;
        if (coma1 != ',' || coma2 != ',' || rt[0] != '$' || rs[0] != '$'){
            cerr << "Error: La instruccion tipo I: " << instruccion << " esta mal estructurada" << endl;
            ans =  false;
        }else{
            // Añadir la instrucción al vector
            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back(rt);
            ins.push_back(inmediato); // Valor inmediato
        }

    } else if (nombre == "lw" || nombre == "sw" || nombre == "lb" || nombre == "lbu" || nombre == "sb" || nombre == "ll" || nombre == "sc"){
        // Formato: rt, offset(rs)
        ss >> rt >> coma1 >> rs;
        if (coma1 != ',' || rt[0] != '$' || rs.find('(') == string::npos || rs.find(')') == string::npos){
            cerr << "Error: La instruccion tipo I: " << instruccion << " esta mal estructurada" << endl;
            ans =  false;
        }else{
            // Extraemos el offset y el registro rs

            char parenAbierto = rs.find('(');
            char parenCerrado = rs.find(')');
            string offset = rs.substr(0, parenAbierto);
            string reg = rs.substr(parenAbierto + 1, parenCerrado - parenAbierto - 1);
    
            if (reg[0] != '$'){
                cerr << "Error: Registro mal formado en la instruccion tipo I: " << instruccion << endl;
                ans =  false;
            }else{
                ins.push_back(nombre);
                ins.push_back(reg);
                ins.push_back(rt);
                ins.push_back(offset); 
            }

        }



    }else if(nombre == "lui"){
        // Formato: rt, inmediato
        ss >> rt >> coma1 >> inmediato;
        if (coma1 != ',' || rt[0] != '$') {
            cerr << "Error: La instruccion tipo I: " << instruccion << " esta mal estructurada" << endl;
            ans = false;
        }else{
            ins.push_back(nombre);
            ins.push_back("00000");
            ins.push_back(rt);
            ins.push_back(inmediato);   
        }
    } else {
        ans =  false;
    }
    return ans;
}

bool validarTipoJ(string &instruccion, vector<string> &ins, map<string, unsigned int> &etiquetas){
    bool ans;
    istringstream ss(instruccion);
    string nombre, etiqueta;

    ss >> nombre >> etiqueta;

    if (etiqueta.empty()){
        cerr << "Error: La instruccion tipo J: " << instruccion << " esta mal estructurada" <<endl;
        ans = false;
    }else{
        if (etiquetas.find(etiqueta) == etiquetas.end()) {
            //cerr << "Error: Etiqueta no encontrada en la instruccion tipo J: " << instruccion << endl;
            ans = false;
        }else{
            ins.push_back(nombre);
            ins.push_back(to_string(etiquetas[etiqueta])); // Convertir la dirección a string
            ans = true;
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
                // Verificar si es una etiqueta
                if (lineaLeida.back() == ':'){
                    string etiqueta = lineaLeida.substr(0, lineaLeida.length() - 1);
                    etiquetas[etiqueta] = pc + (contadorLinea * 4); // Cada instrucción ocupa 4 bytes
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

            string lineaLeida = limpiarLinea(linea);
            if (!lineaLeida.empty()) {
                if (lineaLeida.back() != ':'){ // Ignorar líneas de etiquetas
                    if (validarTipoR(lineaLeida, ins)){
                        pc += 4;
                    } else if (validarTipoJ(lineaLeida, ins, etiquetas)){
                        pc += 4;
                    } else if (validarTipoI(lineaLeida, ins, etiquetas, pc)){
                        pc += 4;
                    } else {
                        cerr << "Error: Instruccion no reconocida: " << lineaLeida << endl;
                    }
                }
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

    vector<string> traduccion = traducir(instrucciones);

    // Guardar la traducción en un archivo de texto
    string nombreArchivoSalida = "traduccionMIPS.txt";
    generarTxt(traduccion, nombreArchivoSalida);

    return 0;
}