#include <iostream>
#include <fstream>
#include <string>

struct Equipo {
    std::string nombre;
    int pts = 0;
    int pj = 0;
    int pg = 0;
    int pe = 0;
    int pp = 0;
    int gf = 0;
    int gc = 0;
};

void mostrarEquipos(Equipo *e, int n) {
    for (int i = 0; i < n; i++) {
        std::cout << i + 1 << ". " << e[i].nombre << "\n";
    }
}

int main() {

    Equipo equipos[7];
    int total = 0;

    std::string liga;
    int ptsV = 3, ptsE = 1, ptsD = 0;

    std::ifstream config("config.txt");
    std::string linea;

    if (!config.is_open()) {
        std::cout << "Error al abrir config.txt\n";
        return 0;
    }

    while (getline(config, linea)) {

        if (linea.find("liga=") == 0)
            liga = linea.substr(5);

        else if (linea.find("victoria=") == 0)
            ptsV = stoi(linea.substr(9));

        else if (linea.find("empate=") == 0)
            ptsE = stoi(linea.substr(7));

        else if (linea.find("derrota=") == 0)
            ptsD = stoi(linea.substr(8));

        else if (linea.find("equipo=") == 0) {
            equipos[total].nombre = linea.substr(7);
            total++;
        }
    }

    config.close();

    int op;

    do {
        std::cout << "\n=== " << liga << " ===\n";
        std::cout << "1. Ver equipos\n";
        std::cout << "2. Registrar partido\n";
        std::cout << "3. Ver tabla\n";
        std::cout << "4. Ver historial jornadas\n";
        std::cout << "5. Salir\n";
        std::cout << "Opcion: ";
        std::cin >> op;

        if (op == 1) {
            mostrarEquipos(equipos, total);
        }

        else if (op == 2) {

            int fecha, l, v, gL, gV;

            mostrarEquipos(equipos, total);

            std::cout << "Fecha (jornada): ";
            std::cin >> fecha;

            std::cout << "Local: ";
            std::cin >> l;
            std::cout << "Visitante: ";
            std::cin >> v;

            l--; v--;

            if (l < 0 || v < 0 || l >= total || v >= total || l == v) {
                std::cout << "Seleccion invalida\n";
                continue;
            }

            std::cout << "Goles local: ";
            std::cin >> gL;
            std::cout << "Goles visitante: ";
            std::cin >> gV;

            std::ofstream p("partidos.txt", std::ios::app);
            std::ofstream f("fechas.txt", std::ios::app);

            if (p.is_open()) {
                p << fecha << ";"
                  << equipos[l].nombre << ";"
                  << equipos[v].nombre << ";"
                  << gL << ";"
                  << gV << "\n";
                p.close();
            }

            if (f.is_open()) {
                f << "JORNADA=" << fecha << "\n";
                f << fecha << ";" << equipos[l].nombre << ";"
                  << equipos[v].nombre << ";" << gL << ";" << gV << "\n";
                f << "FIN_JORNADA\n\n";
                f.close();
            }
        }

        else if (op == 3) {

            for (int i = 0; i < total; i++) {
                equipos[i].pts = 0;
                equipos[i].pj = 0;
                equipos[i].pg = 0;
                equipos[i].pe = 0;
                equipos[i].pp = 0;
                equipos[i].gf = 0;
                equipos[i].gc = 0;
            }

            std::ifstream p("partidos.txt");

            if (!p.is_open()) {
                std::cout << "No hay partidos\n";
                continue;
            }

            while (getline(p, linea)) {

                std::string datos[5] = {"","","","",""};
                int k = 0;

                for (int i = 0; i < linea.length(); i++) {
                    if (linea[i] == ';') k++;
                    else datos[k] += linea[i];
                }

                int gL = 0, gV = 0;

                for (int i = 0; i < datos[3].length(); i++)
                    gL = gL * 10 + (datos[3][i] - '0');

                for (int i = 0; i < datos[4].length(); i++)
                    gV = gV * 10 + (datos[4][i] - '0');

                int posL = -1, posV = -1;

                for (int i = 0; i < total; i++) {
                    if (equipos[i].nombre == datos[1]) posL = i;
                    if (equipos[i].nombre == datos[2]) posV = i;
                }

                if (posL != -1 && posV != -1) {

                    equipos[posL].pj++;
                    equipos[posV].pj++;

                    equipos[posL].gf += gL;
                    equipos[posL].gc += gV;

                    equipos[posV].gf += gV;
                    equipos[posV].gc += gL;

                    if (gL > gV) {
                        equipos[posL].pts += ptsV;
                        equipos[posL].pg++;
                        equipos[posV].pp++;
                    }
                    else if (gV > gL) {
                        equipos[posV].pts += ptsV;
                        equipos[posV].pg++;
                        equipos[posL].pp++;
                    }
                    else {
                        equipos[posL].pts += ptsE;
                        equipos[posV].pts += ptsE;
                        equipos[posL].pe++;
                        equipos[posV].pe++;
                    }
                }
            }

            p.close();

            for (int i = 0; i < total - 1; i++) {
                for (int j = i + 1; j < total; j++) {

                    int dg1 = equipos[i].gf - equipos[i].gc;
                    int dg2 = equipos[j].gf - equipos[j].gc;

                    if (equipos[i].pts < equipos[j].pts ||
                        (equipos[i].pts == equipos[j].pts && dg1 < dg2)) {

                        Equipo temp = equipos[i];
                        equipos[i] = equipos[j];
                        equipos[j] = temp;
                    }
                }
            }

            std::cout << "\nTABLA DE POSICIONES\n";

            std::ofstream t("tabla.txt");

            for (int i = 0; i < total; i++) {

                std::cout << i + 1 << ". "
                          << equipos[i].nombre
                          << " PJ:" << equipos[i].pj
                          << " PG:" << equipos[i].pg
                          << " PE:" << equipos[i].pe
                          << " PP:" << equipos[i].pp
                          << " GF:" << equipos[i].gf
                          << " GC:" << equipos[i].gc
                          << " PTS:" << equipos[i].pts << "\n";

                if (t.is_open()) {
                    t << equipos[i].nombre << " "
                      << equipos[i].pj << " "
                      << equipos[i].pg << " "
                      << equipos[i].pe << " "
                      << equipos[i].pp << " "
                      << equipos[i].gf << " "
                      << equipos[i].gc << " "
                      << equipos[i].pts << "\n";
                }
            }

            t.close();
        }

        else if (op == 4) {

            std::ifstream f("fechas.txt");

            if (!f.is_open()) {
                std::cout << "No hay historial\n";
                continue;
            }

            std::cout << "\nHISTORIAL DE JORNADAS\n\n";

            while (getline(f, linea)) {
                std::cout << linea << "\n";
            }

            f.close();
        }

    } while (op != 5);

    return 0;
}
