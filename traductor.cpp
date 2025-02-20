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

using namespace std;

map<string, vector<string>> intruccionesMips = {
    {"add", {"R", "000000", "100000"}},
    {"addi", {"I", "001000"}},
    {"addiu", {"I", "001001"}},
    {"addu", {"R", "000000", "100001"}},
    {"and", {"R", "000000", "100100"}},
    {"andi", {"I", "001100"}},
    {"beq", {"I", "000100"}},
    {"bne", {"I", "000101"}},
    {"j", {"J", "000010"}},
    {"jal", {"J", "000011"}},
    {"jr", {"R", "000000", "001000"}},
    {"lbu", {"I", "100100"}},
    {"lhu", {"I", "100101"}},
    {"ll", {"I", "110000"}},
    {"lui", {"I", "001111"}},
    {"lw", {"I", "100011"}},
    {"nor", {"R", "000000", "100111"}},
    {"or", {"R", "000000", "100101"}},
    {"ori", {"I", "001101"}},
    {"slt", {"R", "000000", "101010"}},
    {"slti", {"I", "001010"}},
    {"sltiu", {"I", "001011"}},
    {"sltu", {"R", "000000", "101011"}},
    {"sll", {"R", "000000", "000000"}},
    {"srl", {"R", "000000", "000010"}},
    {"sb", {"I", "101000"}},
    {"sc", {"I", "111000"}},
    {"sh", {"I", "101001"}},
    {"sw", {"I", "101011"}},
    {"sub", {"R", "000000", "100010"}},
    {"subu", {"R", "000000", "100011"}},
    {"div", {"R", "000000", "011010"}},
    {"divu", {"R", "000000", "011011"}},
    {"lwc1", {"I", "110001"}},
    {"ldc1", {"I", "110001"}},
    {"mfhi", {"R", "000000", "010000"}},
    {"mflo", {"R", "000000", "010010"}},
    {"mfc0", {"R", "000000", "000000"}},
    {"mult", {"R", "000000", "011000"}},
    {"multu", {"R", "000000", "011001"}},
    {"sra", {"R", "000000", "000011"}},
    {"swcl", {"I", "101011"}},
    {"sdcl", {"I", "101011"}}
};


map<string, string> registroMIPS = {
    {"$0", "00000"},
    {"$zero", "00000"},
    {"$at", "00001"},
    {"$v0", "00010"},
    {"$v1", "00011"},
    {"$a0", "00100"},
    {"$a1", "00101"},
    {"$a2", "00110"},
    {"$a3", "00111"},
    {"$t0", "01000"},
    {"$t1", "01001"},
    {"$t2", "01010"},
    {"$t3", "01011"},
    {"$t4", "01100"},
    {"$t5", "01101"},
    {"$t6", "01110"},
    {"$t7", "01111"},
    {"$s0", "10000"},
    {"$s1", "10001"},
    {"$s2", "10010"},
    {"$s3", "10011"},
    {"$s4", "10100"},
    {"$s5", "10101"},
    {"$s6", "10110"},
    {"$s7", "10111"},
    {"$t8", "11000"},
    {"$t9", "11001"},
    {"$k0", "11010"},
    {"$k1", "11011"},
    {"$gp", "11100"},
    {"$sp", "11101"},
    {"$fp", "11110"},
    {"$ra", "11111"}
};

vector<string> leerArchivo(string& nombreArchivo) {
    ifstream archivo(nombreArchivo); // Abre el archivo
    vector<string> contenido; // Vector para almacenar las líneas

    string linea;
    while (std::getline(archivo, linea)) { // Lee línea por línea
        if (!linea.empty() && linea[0] == '#') {
            continue; // Ignorar la línea si comienza con #
        }
        contenido.push_back(linea); // Agregar solo las líneas válidas
    }

    archivo.close(); // Cierra el archivo
    return contenido; // Retorna el contenido del archivo
}









int main() {
    string nombre;
    cout << " nombre archivo ";
    cin >> nombre;

    vector<string> contenido = leerArchivo(nombre); // llamamos la funcion


    return 0;
}
