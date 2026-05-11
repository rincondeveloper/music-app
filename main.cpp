#include <iostream>
#include <string>
#ifdef _WIN32
  #include <windows.h>
#endif
using namespace std;

// Códigos ANSI 
#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define GRAY    "\033[90m"
#define WHITE   "\033[97m"
#define DIM     "\033[2m"

// ─── Utilidades ───
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar() {
    cout << GRAY << "\n  Presiona Enter para continuar..." << RESET;
    cin.ignore();
    cin.get();
}

void separador() {
    cout << DIM << "══════════════════════════════════════════════════" << RESET << "\n";
}

// Hero
void hero() {
    cout << CYAN << "  ██╗ █████╗ ███╗   ███╗███╗   ███╗██╗   ██╗███████╗██╗ ██████╗ " << RESET << "\n";
    cout << CYAN << "  ██║██╔══██╗████╗ ████║████╗ ████║██║   ██║██╔════╝██║██╔════╝ " << RESET << "\n";
    cout << CYAN << "  ██║███████║██╔████╔██║██╔████╔██║██║   ██║███████╗██║██║      " << RESET << "\n";
    cout << CYAN << "  ██║██╔══██║██║╚██╔╝██║██║╚██╔╝██║██║   ██║╚════██║██║██║      " << RESET << "\n";
    cout << CYAN << "  ██║██║  ██║██║ ╚═╝ ██║██║ ╚═╝ ██║╚██████╔╝███████║██║╚██████╗" << RESET << "\n";
    cout << CYAN << "  ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝     ╚═╝ ╚═════╝ ╚══════╝╚═╝ ╚═════╝" << RESET << "\n";
}

// ─── Menús ────
void menuBiblioteca() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        hero();
        cout << "\n\t\t\t  " << CYAN << "♪" << RESET << "  " << WHITE << "BIBLIOTECA" << RESET << "\n";

        cout << "\n  " << CYAN << "[1]" << RESET << WHITE << "  Registrar artista\n"   << RESET;
        cout << "  " << CYAN << "[2]" << RESET << WHITE << "  Registrar álbum\n"     << RESET;
        cout << "  " << CYAN << "[3]" << RESET << WHITE << "  Registrar canción\n"   << RESET;
        cout << "  " << CYAN << "[4]" << RESET << WHITE << "  Ver biblioteca completa\n" << RESET;
        cout << "  " << RED    << "[5]" << RESET << RED   << "  Eliminar artista\n"    << RESET;
        cout << "\n  " << DIM  << "[0]" << RESET << DIM   << "  Regresar al menú principal\n" << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: cout << GREEN << "\n  [Registrar artista — por implementar]\n" << RESET; pausar(); break;
            case 2: cout << GREEN << "\n  [Registrar álbum — por implementar]\n"   << RESET; pausar(); break;
            case 3: cout << GREEN << "\n  [Registrar canción — por implementar]\n" << RESET; pausar(); break;
            case 4: cout << GREEN << "\n  [Ver biblioteca — por implementar]\n"    << RESET; pausar(); break;
            case 5: cout << RED   << "\n  [Eliminar artista — por implementar]\n"  << RESET; pausar(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

void menuReproductor() {
    string cancionActual = "Karma Police — Radiohead";
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        hero();
        cout << "\n\t\t\t  " << CYAN << "▶" << RESET << "  " << WHITE << "REPRODUCTOR" << RESET << "\n";
        cout << "\n  " << GREEN << "♪  " << cancionActual << RESET << "\n";
        cout << "\n" << DIM << "  ─────────────────────────────────────────────\n" << RESET;
        cout << "  " << CYAN << "[1]" << RESET << WHITE << "  Reproducir canción\n"        << RESET;
        cout << "  " << CYAN << "[2]" << RESET << WHITE << "  Agregar canción a la cola\n" << RESET;
        cout << "  " << CYAN << "[3]" << RESET << WHITE << "  Siguiente  ⏭\n"              << RESET;
        cout << "  " << CYAN << "[4]" << RESET << WHITE << "  Anterior   ⏮\n"              << RESET;
        cout << "  " << CYAN << "[5]" << RESET << WHITE << "  Ver historial\n"             << RESET;
        cout << "  " << CYAN << "[6]" << RESET << WHITE << "  Ver cola\n"                  << RESET;
        cout << "\n  " << DIM  << "[0]" << RESET << DIM   << "  Regresar al menú principal\n" << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: cout << GREEN << "\n  [Reproducir canción — por implementar]\n"        << RESET; pausar(); break;
            case 2: cout << GREEN << "\n  [Agregar a cola — por implementar]\n"            << RESET; pausar(); break;
            case 3: cout << GREEN << "\n  [Siguiente canción — por implementar]\n"         << RESET; pausar(); break;
            case 4: cout << GREEN << "\n  [Canción anterior — por implementar]\n"          << RESET; pausar(); break;
            case 5: cout << GREEN << "\n  [Ver historial — por implementar]\n"             << RESET; pausar(); break;
            case 6: cout << GREEN << "\n  [Ver cola — por implementar]\n"                  << RESET; pausar(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

void menuBuscador() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        hero();
        cout << "\n  " << CYAN << "🔎" << RESET << "  " << WHITE << "BUSCADOR" << RESET << "\n";
        cout << "\n  " << CYAN << "[1]" << RESET << WHITE << "  Buscar canción por título  " << DIM << "(en arreglo)\n" << RESET;
        cout << "  " << CYAN << "[2]" << RESET << WHITE << "  Buscar canción por título  " << DIM << "(en CSV)\n"     << RESET;
        cout << "  " << CYAN << "[3]" << RESET << WHITE << "  Buscar álbumes por año\n"                               << RESET;
        cout << "\n  " << DIM  << "[0]" << RESET << DIM   << "  Regresar al menú principal\n" << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: cout << GREEN << "\n  [Búsqueda lineal en arreglo — por implementar]\n" << RESET; pausar(); break;
            case 2: cout << GREEN << "\n  [Búsqueda lineal en CSV — por implementar]\n"     << RESET; pausar(); break;
            case 3: cout << GREEN << "\n  [Ordenar + búsqueda binaria — por implementar]\n" << RESET; pausar(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

void menuPlaylist() {
    string tipoActivo = "Simple Lineal Sin Enc.";
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        separador();
        cout << "  " << CYAN << "≡" << RESET << "  " << WHITE << "PLAYLIST  " << DIM << "[ tipo: " << tipoActivo << " ]" << RESET << "\n";
        separador();
        cout << "\n" << GRAY << "  Selecciona una opción:\n" << RESET << "\n";
        cout << "  " << YELLOW << "[1]" << RESET << WHITE << "  Cambiar tipo de lista\n"                                          << RESET;
        cout << "  " << YELLOW << "[2]" << RESET << WHITE << "  Agregar canción\n"                                                << RESET;
        cout << "  " << YELLOW << "[3]" << RESET << WHITE << "  Eliminar canción\n"                                               << RESET;
        cout << "  " << YELLOW << "[4]" << RESET << WHITE << "  Siguiente  ⏭\n"                                                   << RESET;
        cout << "  " << YELLOW << "[5]" << RESET << WHITE << "  Anterior   ⏮  " << DIM << "(solo listas dobles)\n"               << RESET;
        cout << "  " << YELLOW << "[6]" << RESET << WHITE << "  Ver playlist completa\n"                                          << RESET;
        cout << "\n  " << DIM  << "[0]" << RESET << DIM   << "  Regresar al menú principal\n"                                    << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: cout << GREEN << "\n  [Cambiar tipo de lista — por implementar]\n" << RESET; pausar(); break;
            case 2: cout << GREEN << "\n  [Agregar canción — por implementar]\n"       << RESET; pausar(); break;
            case 3: cout << GREEN << "\n  [Eliminar canción — por implementar]\n"      << RESET; pausar(); break;
            case 4: cout << GREEN << "\n  [Siguiente — por implementar]\n"             << RESET; pausar(); break;
            case 5: cout << GREEN << "\n  [Anterior — por implementar]\n"              << RESET; pausar(); break;
            case 6: cout << GREEN << "\n  [Ver playlist — por implementar]\n"          << RESET; pausar(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

void menuExplorar() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        separador();
        cout << "  " << CYAN << "T" << RESET << "  " << WHITE << "EXPLORAR BIBLIOTECA" << RESET << "\n";
        separador();
        cout << "\n" << GRAY << "  Selecciona una opción:\n" << RESET << "\n";
        cout << "  " << YELLOW << "[1]" << RESET << WHITE << "  Ver árbol completo\n"                      << RESET;
        cout << "  " << YELLOW << "[2]" << RESET << WHITE << "  Navegar y agregar canción a la cola\n"     << RESET;
        cout << "\n  " << DIM  << "[0]" << RESET << DIM   << "  Regresar al menú principal\n"             << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: cout << GREEN << "\n  [Ver árbol — por implementar]\n"                  << RESET; pausar(); break;
            case 2: cout << GREEN << "\n  [Navegar y agregar a cola — por implementar]\n"   << RESET; pausar(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

void menuGeneros() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        separador();
        cout << "  " << CYAN << "◈" << RESET << "  " << WHITE << "GÉNEROS RELACIONADOS" << RESET << "\n";
        separador();
        cout << "\n" << GRAY << "  Selecciona una opción:\n" << RESET << "\n";
        cout << "  " << YELLOW << "[1]" << RESET << WHITE << "  Registrar relación entre géneros\n" << RESET;
        cout << "  " << YELLOW << "[2]" << RESET << WHITE << "  Ver matriz de adyacencia\n"          << RESET;
        cout << "  " << YELLOW << "[3]" << RESET << WHITE << "  Ver recomendaciones\n"               << RESET;
        cout << "\n  " << DIM  << "[0]" << RESET << DIM   << "  Regresar al menú principal\n"      << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: cout << GREEN << "\n  [Registrar relación — por implementar]\n" << RESET; pausar(); break;
            case 2: cout << GREEN << "\n  [Ver matriz — por implementar]\n"          << RESET; pausar(); break;
            case 3: cout << GREEN << "\n  [Ver recomendaciones — por implementar]\n" << RESET; pausar(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

void menuArchivos() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        separador();
        cout << "  " << CYAN << "»" << RESET << "  " << WHITE << "ARCHIVOS" << RESET << "\n";
        separador();
        cout << "\n" << GRAY << "  Selecciona una opción:\n" << RESET << "\n";
        cout << "  " << YELLOW << "[1]" << RESET << WHITE << "  Guardar datos\n" << RESET;
        cout << DIM  << "        artistas.bin  |  albumes.csv  |  canciones.txt\n" << RESET;
        cout << "\n";
        cout << "  " << YELLOW << "[2]" << RESET << WHITE << "  Cargar datos\n" << RESET;
        cout << DIM  << "        artistas.bin  |  albumes.csv  |  canciones.txt\n" << RESET;
        cout << "\n  " << DIM  << "[0]" << RESET << DIM << "  Regresar al menú principal\n" << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: cout << GREEN << "\n  [Guardar datos — por implementar]\n" << RESET; pausar(); break;
            case 2: cout << GREEN << "\n  [Cargar datos — por implementar]\n"  << RESET; pausar(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

// ─── Menú Principal ──────────────────────────────────────────────────────────
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        hero();
        cout << "\n  " << CYAN << "[1]" << RESET << WHITE << "  Biblioteca\n"            << RESET;
        cout << "  " << CYAN << "[2]" << RESET << WHITE << "  Reproductor\n"           << RESET;
        cout << "  " << CYAN << "[3]" << RESET << WHITE << "  Buscador\n"              << RESET;
        cout << "  " << CYAN << "[4]" << RESET << WHITE << "  Playlist\n"              << RESET;
        cout << "  " << CYAN << "[5]" << RESET << WHITE << "  Explorar biblioteca\n"   << RESET;
        cout << "  " << CYAN << "[6]" << RESET << WHITE << "  Géneros relacionados\n"  << RESET;
        cout << "  " << CYAN << "[7]" << RESET << WHITE << "  Archivos\n"              << RESET;
        cout << "\n  " << RED  << "[0]" << RESET << RED   << "  Salir\n"                 << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: menuBiblioteca(); break;
            case 2: menuReproductor(); break;
            case 3: menuBuscador(); break;
            case 4: menuPlaylist(); break;
            case 5: menuExplorar(); break;
            case 6: menuGeneros(); break;
            case 7: menuArchivos(); break;
            case 0:
                limpiarPantalla();
                cout << "  " << CYAN << "[0]" << RESET << "  Cancelar\n" << RESET;
                cout << "  " << RED << "[1]" << RESET << WHITE << "  Confirmar salida\n" << RESET;
                cout << "\n"; separador();
                cout << GRAY << "  >> " << RESET;
                int conf; 
                cin >> conf;
                if (conf == 1) {
                    limpiarPantalla();
                    return 0;
                } else {
                    op = -1;
                }
                break;
            default:
                cout << RED << "\n  Opción inválida.\n" << RESET;
                pausar();
        }
    }
    return 0;
}