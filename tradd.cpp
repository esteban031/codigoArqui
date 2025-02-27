#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <bitset>  

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

vector<string> registrosProtegidos = {"$0","$zero", "$gp", "$k0", "$k1"};

int var = 4; 

int calcularDesplazamiento(int direccionEtiqueta, int pc){
    int offset = (direccionEtiqueta - (pc + var)) / var;
    return offset;
}

int calcularDesplazamientoTipoJ(int direccionEtiqueta, int pc){
    int ans = direccionEtiqueta / 4;
    return ans;
}

bool esHexadecimal(const string &valor) {
    return (valor.rfind("0x", 0) == 0 || valor.rfind("0X", 0) == 0);
}

bool esRegistroProtegido(const string &registro) {
    // Verifica si el registro esta en la lista de registros protegidos
    return find(registrosProtegidos.begin(), registrosProtegidos.end(), registro) != registrosProtegidos.end();
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


string convertirABinario(int valor, int bits) {
    if (valor < 0) {
        // Si el valor es negativo, lo convertimos a complemento a dos
        valor = (1 << bits) + valor;
    }

    // Convertimos el valor a binario con el número de bits especificado
    string binario;
    for (int i = bits - 1; i >= 0; i--) {
        if ((valor >> i) & 1) {
            binario += '1';
        } else {
            binario += '0';
        }
    }

    // Asegurarse de que el resultado tenga exactamente 'bits' bits
    while (binario.length() < bits) {
        binario = '0' + binario;
    }

    return binario;
}

string hexABinario(const string& hexStr) {
    string cleanHex = (hexStr.rfind("0x", 0) == 0) ? hexStr.substr(2) : hexStr;
    unsigned long num;
    stringstream ss;
    ss << hex << cleanHex;
    ss >> num;
    bitset<16> bin(num);
    return bin.to_string();
}

bool esBinario(const string &numero) {
    // Verificar que todos los caracteres sean '0' o '1'
    return all_of(numero.begin(), numero.end(), [](char c) {
        return c == '0' || c == '1';
    });
}

vector<string> traducir(vector<string> ins) {
    vector<string> resultado;
    int i = 0;
    while (i < ins.size()) {
        string nombre = ins[i]; // Nombre de la instrucción

        if (instrucciones.find(nombre) != instrucciones.end()) {
            string opcode = instrucciones[nombre].first; // Opcode de la instrucción
            string funct = instrucciones[nombre].second; // Funct solo para tipo R
            if (opcode == "000000") {
                string rs, rt, rd, shamt;

                // Handle specific R-type instructions
                if (nombre == "jr" || nombre == "mflo" || nombre == "mfhi"){
                    // Only rs is used
                    rs = registros[ins[i + 1]];
                    rt = "00000";
                    rd = "00000";
                    shamt = "00000";
                } else if (nombre == "sll" || nombre == "srl" || nombre == "sra") {
                    // Only rt, rd, and shamt are used
                    rs = "00000";
                    rt = registros[ins[i + 2]];
                    rd = registros[ins[i + 3]];
                    shamt = ins[i + 4];
                } else if (nombre == "mult" || nombre == "multu" || nombre == "div" || nombre == "divu") {
                    // Only rs and rt are used
                    rs = registros[ins[i + 1]];
                    rt = registros[ins[i + 2]];
                    rd = "00000";
                    shamt = "00000";
                }else{
                    // Default case: rs, rt, and rd are used
                    rs = registros[ins[i + 1]];
                    rt = registros[ins[i + 2]];
                    rd = registros[ins[i + 3]];
                    shamt = "00000";
                }

                string binario = opcode + rs + rt + rd + shamt + funct;
                resultado.push_back(binario);

                i += 5; // Avanzar al siguiente grupo de instrucciones
            } else if (nombre == "j" || nombre == "jal") { // Instrucción tipo J
                string direccion = ins[i + 1];

                // Convertir la dirección a binario de 26 bits
                int direccionEntero = stoi(direccion); // Convertir a entero
                string direccionBinario = convertirABinario(direccionEntero, 26);

                // Construir la instrucción en binario
                string binario = opcode + direccionBinario;
                resultado.push_back(binario);

                i += 2; // Avanzar al siguiente grupo de instrucciones
            } else { // Instrucción tipo I
                string rs = registros[ins[i + 1]];
                string rt = registros[ins[i + 2]];
                string inmediato = ins[i + 3];
                string inmediatoBinario;
                if(nombre == "lui"){
                    rs = "00000";
                }

                /* cout << "FUCK"<< endl;
                cout << "rs: "<< rs << "rt :" << rt << endl;

                cout << "------"<< endl; */

                if (inmediato == "0") {
                    // Si el inmediato es 0, llenar con 16 bits de 0
                    inmediatoBinario = "0000000000000000";
                } else if (!esBinario(inmediato)) {
                    // Convertir el inmediato a binario de 16 bits
                    int inmediatoEntero = stoi(inmediato); // Convertir a entero
                    inmediatoBinario = convertirABinario(inmediatoEntero, 16);
                } else {
                    inmediatoBinario = inmediato; // Ya está en binario
                }

                // Construir la instrucción en binario
                string binario = opcode + rs + rt + inmediatoBinario;
                resultado.push_back(binario);

                i += 4; // Avanzar al siguiente grupo de instrucciones
            }
        } else {
            cerr << "Error: Instruccion no reconocida: " << nombre << endl;
            i++; // Avanzar a la siguiente instrucción
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
        }else {
            cerr << "Error: Registros no validos en la instruccion: " << instruccion << endl;
        }
    } else if (nombre == "jr" || nombre == "mfhi" || nombre == "mflo") {
        string rs;
        ss >> rs;


        if (esRegistroProtegido(rs)) {
            cerr << "Error: No se puede modificar el registro protegido: " << rs << endl;
            ans = false;
        }else{
            ans = esRegistroValido(rs);
            if (ans) {
                ins.push_back(nombre);
                ins.push_back(rs);
                ins.push_back("00000");
                ins.push_back("00000");
                ins.push_back("00000");
            } else {
                cerr << "Error: Registros no validos en la instruccion: " << instruccion << endl;
            }     
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
        else {
            cerr << "Error: Registros no validos en la instruccion: " << instruccion << endl;
        }
    } else if (nombre == "sll" || nombre == "srl" || nombre == "sra") {
        string rd, rt, shamt;
        ss >> rd >> rt >> shamt;

        if (esRegistroProtegido(rd)) {
            cerr << "Error: No se puede modificar el registro protegido: " << rd << endl;
            ans = false;
        }else{
            ans = esRegistroValido(rd) && esRegistroValido(rt);
            if (ans) {
                ins.push_back(nombre);
                ins.push_back("00000");
                ins.push_back(rt);
                ins.push_back(rd);
                ins.push_back(shamt);
            }else {
                cerr << "Error: Registros no validos en la instruccion: " << instruccion << endl;
            }
        }

    } else if (nombre == "add" || nombre == "addu" || nombre == "sub" || nombre == "subu" ||
               nombre == "and" || nombre == "or" || nombre == "nor" || nombre == "slt" || nombre == "sltu") {
        string rd, rs, rt;
        ss >> rd >> rs >> rt;
        
        if (esRegistroProtegido(rd)) {
            cerr << "Error: No se puede modificar el registro protegido: " << rd << endl;
            ans = false;
        }else{
            ans = esRegistroValido(rd) && esRegistroValido(rs) && esRegistroValido(rt);
            if (ans) {
                ins.push_back(nombre);
                ins.push_back(rs);
                ins.push_back(rt);
                ins.push_back(rd);
                ins.push_back("00000");
            }
            else {
                cerr << "Error: Registros no validos en la instruccion: " << instruccion << endl;
            }

        }
        //cout << rd << endl << rs << endl << rt;
    }
    return ans;
}

bool validarTipoI(string &instruccion, vector<string> &ins, map<string, unsigned int> &etiquetas, int pc) {
    bool ans = false;
    istringstream ss(instruccion);
    string nombre;
    ss >> nombre;

    // Reemplazar comas por espacios en la instrucción
    for (int i = 0; i < instruccion.length();i++) {
        if (instruccion[i] == ','){
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
            string offsetBinario;

            if(esHexadecimal(etiqueta)){
                offsetBinario = hexABinario(etiqueta);
            }else{
                int offset = calcularDesplazamiento(etiquetas[etiqueta], pc);
                /* cout << "OFFSET" << endl;
                cout << offset << endl; */
                offsetBinario = convertirABinario(offset, 16); // Cambio aquí
            }

            ins.push_back(nombre);
            ins.push_back(rs);
            ins.push_back(rt);
            ins.push_back(offsetBinario);
            
        } else {
            cerr << "Error: Registros o etiqueta no validos en la instruccion: " << instruccion << endl;
        }
    } else if (nombre == "addi" || nombre == "addiu" || nombre == "andi" || nombre == "slti" || nombre == "sltiu") {
        // Formato: rt rs inmediato
        string rt, rs, inmediato;
        ss >> rt >> rs >> inmediato;

        if (esRegistroProtegido(rt)) {
            cerr << "Error: No se puede modificar el registro protegido: " << rt << endl;
            ans = false;
        }else{
            // Validar registros
            ans = esRegistroValido(rt) && esRegistroValido(rs);
        
            if (ans){
                ins.push_back(nombre);
                ins.push_back(rs);
                ins.push_back(rt);
                /* if(nombre == "ori"){
                    inmediato = hexABinario(inmediato);   
                } */
                ins.push_back(inmediato);
            }else{
                cerr << "Error: Registros no validos en la instruccion: " << instruccion << endl;
            }
        }
    }else if (nombre == "ori") {
        // Formato: rt rs inmediato (hexadecimal)
        string rt, rs, inmediato;
        ss >> rt >> rs >> inmediato;

        if (esRegistroProtegido(rt)) {
            cerr << "Error: No se puede modificar el registro protegido: " << rt << endl;
            ans = false;
        } else {
            // Validar registros
            ans = esRegistroValido(rt) && esRegistroValido(rs);

            if (ans) {  
                // Validar que el inmediato esté en formato hexadecimal (0x...);
                string inmediatoBinario;
                if(esHexadecimal(inmediato)){
                    // Convertir el valor hexadecimal a entero
                    inmediatoBinario = hexABinario(inmediato);
                }else{
                    inmediatoBinario = convertirABinario(stoi(inmediato), 16);

                }
            
                    ins.push_back(nombre);
                    ins.push_back(rs);
                    ins.push_back(rt);
                    ins.push_back(inmediatoBinario);
                    
                
                
            }else{
                cerr << "Error: Registros no validos en la instruccion: " << instruccion << endl;
            }
        }
    }else if (nombre == "lw" || nombre == "sw" || nombre == "lb" || nombre == "lbu" || nombre == "sb" || nombre == "ll" || nombre == "sc") {
        // Formato: rt offset(rs)
        string rt, rs;
        ss >> rt >> rs;

        if (esRegistroProtegido(rt)) {
            cerr << "Error: No se puede modificar el registro protegido: " << rt << endl;
            ans = false;
        }else{
            // Extraer offset y registro base
            size_t parenAbierto = rs.find('(');
            size_t parenCerrado = rs.find(')');
            if (parenAbierto == string::npos || parenCerrado == string::npos) {
                cerr << "Error: Formato incorrecto en la instruccion: " << instruccion << endl;
                ans = false;
            } else {
                string offset = rs.substr(0, parenAbierto);
                string reg = rs.substr(parenAbierto + 1, parenCerrado - parenAbierto - 1);
                if(esHexadecimal(offset)){
                    offset = hexABinario(offset);
                }                
                // Validar registros
                ans = esRegistroValido(rt) && esRegistroValido(reg);
                
                if (ans) {
                    ins.push_back(nombre);
                    ins.push_back(reg);
                    ins.push_back(rt);
                    ins.push_back(offset);
                } else {
                    cerr << "Error: Registros no válidos en la instruccion: " << instruccion << endl;
                }
            }
        }
    } else if (nombre == "lui") {
        // Formato: rt inmediato
        string rt, inmediato;
        ss >> rt >> inmediato;

        if (esRegistroProtegido(rt)) {
            cerr << "Error: No se puede usar el registro protegido: " << rt << " en lui" << endl;
            ans = false;
        }else{
            // Validar registro
            ans = esRegistroValido(rt);
    
            if (ans) {
                string inmediatoBinario;
                // Validar que el inmediato esté en formato hexadecimal (0x...)
                if(esHexadecimal(inmediato)){
                    inmediatoBinario = hexABinario(inmediato);
                }

                // Agregar a la lista de instrucciones
                ins.push_back(nombre);
                ins.push_back("00000"); // rs no se usa en lui
                ins.push_back(rt);
                ins.push_back(inmediatoBinario);
                    
            } else {
                cerr << "Error: El inmediato debe estar en formato hexadecimal (0x...): " << inmediato << endl;
                ans = false;
            }
        }
    }else{
        cerr << "Error: Instruccion no reconocida: " << instruccion << endl;
        ans = false;
    }
    return ans;
}

bool validarTipoJ(string &instruccion, vector<string> &ins, map<string, unsigned int> &etiquetas) {
    bool ans = false;
    istringstream ss(instruccion);
    string nombre, etiqueta;

    ss >> nombre >> etiqueta;

    if (etiqueta.empty()){
        cerr << "Error: La instruccion tipo J: " << instruccion << " esta mal estructurada" << endl;
        ans = false;
    } else if (nombre == "j" || nombre == "jal"){
        if (etiquetas.find(etiqueta) == etiquetas.end()){
            cerr << "Error: Etiqueta no encontrada: " << etiqueta << endl;
            ans = false;
        }else{
            ans = true;
            ins.push_back(nombre);
            if(esHexadecimal(etiqueta)){
                string inmediatoBinario = hexABinario(etiqueta);
                ins.push_back(inmediatoBinario);
            }else{
                ins.push_back(to_string(etiquetas[etiqueta]));
            }
            

            
        }
    }
    return ans;
}

void leerEtiquetas(string nombre, map<string,unsigned int> &etiquetas, unsigned int &pc){
    ifstream archivo(nombre);
    string linea;
    int contadorLinea = 1;

    if (!archivo.is_open()){
        cerr << "Error! No se pudo abrir el archivo " << nombre << endl;
    }else{
        while (getline(archivo, linea)) {
            string lineaLeida = limpiarLinea(linea);
            if (!lineaLeida.empty()) {
                if (lineaLeida.back() == ':'){
                    string etiqueta = lineaLeida.substr(0, lineaLeida.length() - 1);
                    
                    etiquetas[etiqueta] = pc + contadorLinea * var;
                }else{
                    contadorLinea++;
                }
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
            cout << lineaLeida<< endl;
            if (!lineaLeida.empty()) {
                if (lineaLeida.back() != ':'){ // Ignorar líneas de etiquetas
                    if (validarTipoR(lineaLeida, ins)){
                        /* cout << "instruccion:::  " << endl;
                        for(int i = 0; i < ins.size(); i++){
                            cout << ins[i] << " " ;
                        } */
                        pc += var;
                    } else if (validarTipoJ(lineaLeida, ins, etiquetas)){
                        /* cout << "instruccion:::  " << endl;
                        for(int i = 0; i < ins.size(); i++){
                            cout << ins[i] << " " ;
                        } */
                        pc += var;
                    } else if (validarTipoI(lineaLeida, ins, etiquetas, pc)){
                        /* cout << "instruccion:::  " << endl;
                        for(int i = 0; i < ins.size(); i++){
                            cout << ins[i] << " ";
                        } */
                        pc += var;
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

void formatoEntrada(unsigned int &pc) {
    int option;
    string input;

    cout << "Seleccione el formato de entrada:\n";
    cout << "1. Hexadecimal (0x...)\n";
    cout << "2. Decimal\n";
    cout << "Opcion: ";
    cin >> option;

    switch (option) {
        case 1: {
            cout << "Ingrese la direccion en formato hexadecimal (minimo valor 0x00040000): ";
            cin >> input;

            // Verificar si la entrada comienza con "0x" o "0X"
            if (input.find("0x") == 0 || input.find("0X") == 0) {
                // Convertir el valor hexadecimal a decimal
                stringstream ss;
                ss << hex << input; // Leer el valor hexadecimal
                ss >> pc;           // Convertirlo a decimal
            } else {
                cerr << "Error: Formato incorrecto. Debe comenzar con 0x.\n";
                return;
            }

            // Validar que el valor sea mayor o igual a 0x00040000 (262144 en decimal)
            if (pc < 0x00040000) {
                cerr << "Error: La direccion debe ser mayor o igual a 0x00040000.\n";
                return;
            }

            break;
        }
        case 2: {
            cout << "Ingrese la direccion en formato decimal (minimo valor 262144): ";
            cin >> pc;

            // Validar que el valor sea mayor o igual a 262144
            if (pc < 262144) {
                cerr << "Error: La direccion debe ser mayor o igual a 262144.\n";
                return;
            }

            break;
        }
        default: {
            cerr << "Opcion no valida. Intente nuevamente.\n";
            return;
        }
    }

    cout << "Direccion ingresada: " << pc << " (decimal)\n";
}


int main(){
    string nombre;
    unsigned int pc;
    cout << "Ingrese el nombre del archivo que desea traducir: ";
    cin >> nombre ;
    

    formatoEntrada(pc);
    map<string, unsigned int> etiquetas;
    vector<string> instrucciones;
    leerTxt(nombre, instrucciones,pc, etiquetas);
/* 
    for(int i = 0; i < instrucciones.size(); i++){
        cout << instrucciones[i] << " - " ;
    }

    cout << endl; */

    vector<string> traduccion = traducir(instrucciones);
   /*  for(int i = 0; i < traduccion.size(); i++){
        cout << traduccion[i] << " - " ;
    }

    cout << endl; */

    // Guardar la traducción en un archivo de texto
    string nombreArchivoSalida = "traduccionMIPS.txt";
    generarTxt(traduccion, nombreArchivoSalida);

    return 0;
}