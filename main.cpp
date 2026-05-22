#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#ifdef _WIN32
  #include <windows.h>
#endif

using namespace std;

// ─── COLORES ────
#define RESET  "\033[0m"
#define CYAN   "\033[36m"
#define YELLOW "\033[33m"
#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define GRAY   "\033[90m"
#define WHITE  "\033[97m"
#define DIM    "\033[2m"

// ─── STRUCTS ───
struct Cancion {
    int id;
    char titulo[50];
    int numeroPista;
};

struct Album {
    int id;
    char titulo[50];
    char genero[30];
    int anio;
    Cancion canciones[10];
    int totalCanciones;
};

struct Artista {
    int id;
    char nombre[50];
    Album albumes[10];
    int totalAlbumes;
};

// ─── ARREGLO GLOBAL ────
const int MAX_ARTISTAS = 10;
Artista artistas[MAX_ARTISTAS]; 
int totalArtistas = 0;

// ─── PILA (ir a la canción anterior) ────
struct NodoPila {
    Cancion cancion;
    string nombreArtista;
    string nombreAlbum;
    NodoPila* siguiente;
};

NodoPila* topePila = nullptr;

void pushPila(Cancion c, string artista, string album) {
    NodoPila* nuevo = new NodoPila();
    nuevo->cancion = c;
    nuevo->nombreArtista = artista;
    nuevo->nombreAlbum = album;
    nuevo->siguiente = topePila;
    topePila = nuevo;
}

bool popPila(Cancion& c, string& artista, string& album) {
    if (!topePila) { return false; }

    c = topePila->cancion;
    artista = topePila->nombreArtista;
    album = topePila->nombreAlbum;

    NodoPila* temp = topePila;
    topePila = topePila->siguiente;

    delete temp;
    return true;
}

// ─── COLA (siguiente canción) ───
struct NodoCola {
    Cancion cancion;
    string nombreArtista;
    string nombreAlbum;
    NodoCola* siguiente;
};

NodoCola* frenteCola = nullptr;
NodoCola* finalCola  = nullptr;

void enqueueCola(Cancion c, string artista, string album) {
    NodoCola* nuevo = new NodoCola();

    nuevo->cancion = c;
    nuevo->nombreArtista = artista;
    nuevo->nombreAlbum = album;
    nuevo->siguiente = nullptr;

    if (!finalCola) { 
        frenteCola = finalCola = nuevo; 
    } else { 
        finalCola->siguiente = nuevo; finalCola = nuevo; 
    }
}

bool dequeueCola(Cancion& c, string& artista, string& album) {
    if (!frenteCola) { return false; }

    c = frenteCola->cancion;
    artista = frenteCola->nombreArtista;
    album   = frenteCola->nombreAlbum;

    NodoCola* temp = frenteCola;
    frenteCola = frenteCola->siguiente;

    if (!frenteCola) { finalCola = nullptr; }

    delete temp;
    return true;
}

// ─── CANCIÓN ACTUAL ───
Cancion cancionActual;
string artistaActual = "";
string albumActual = "";
bool hayReproduccion = false;

// LISTAS ENLAZADAS
struct NodoLista {
    Cancion cancion;
    NodoLista* siguiente;
    NodoLista* anterior; // para listas dobles
};

struct Encabezado {
    char titulo[50]; // nombre del álbum
    char artista[50];
    char genero[30];
    int anio;
    int totalCanciones;
};

// ── Lista Simple Lineal Sin Encabezado ──
struct ListaSLS {
    NodoLista* cabeza;
    ListaSLS() : cabeza(nullptr) {}

    void insertar(Cancion c) {
        NodoLista* nuevo = new NodoLista{c, nullptr, nullptr};

        if (!cabeza) { 
            cabeza = nuevo; 
            return; 
        }

        NodoLista* tmp = cabeza;
        while (tmp->siguiente) {
            tmp = tmp->siguiente;
        }

        tmp->siguiente = nuevo;
    }

    NodoLista* siguiente(NodoLista* actual) {
        return actual ? actual->siguiente : nullptr;
    }

    void liberar() {
        while (cabeza) { 
            NodoLista* t = cabeza; 
            cabeza = cabeza->siguiente; 
            delete t; 
        }
    }
};

// ── Lista Simple Circular Sin Encabezado ──
struct ListaSCS {
    NodoLista* cabeza;
    ListaSCS() : cabeza(nullptr) {}

    void insertar(Cancion c) {
        NodoLista* nuevo = new NodoLista{c, nullptr, nullptr};

        if (!cabeza) { 
            cabeza = nuevo; 
            nuevo->siguiente = cabeza;
            return; 
        }

        NodoLista* tmp = cabeza;
        while (tmp->siguiente != cabeza) {
            tmp = tmp->siguiente;
        }

        tmp->siguiente = nuevo;
        nuevo->siguiente = cabeza;
    }

    NodoLista* siguiente(NodoLista* actual) {
        return actual ? actual->siguiente : nullptr;
    }

    void liberar() {
        if (!cabeza) { return; }

        NodoLista* tmp = cabeza->siguiente;
        while (tmp != cabeza) { 
            NodoLista* t = tmp; 
            tmp = tmp->siguiente; 
            delete t; 
        }

        delete cabeza; 
        cabeza = nullptr;
    }
};

// ── Lista Doble Lineal Sin Encabezado ──
struct ListaDLS {
    NodoLista* cabeza;
    NodoLista* cola;
    ListaDLS() : cabeza(nullptr), cola(nullptr) {}

    void insertar(Cancion c) {
        NodoLista* nuevo = new NodoLista{c, nullptr, nullptr};

        if (!cabeza) { 
            cabeza = cola = nuevo; 
            return; 
        }

        nuevo->anterior = cola;
        cola->siguiente = nuevo;
        cola = nuevo;
    }

    NodoLista* siguiente(NodoLista* actual) { 
        return actual ? actual->siguiente : nullptr; 
    }

    NodoLista* anterior(NodoLista* actual)  { 
        return actual ? actual->anterior  : nullptr; 
    }

    void liberar() {
        while (cabeza) { 
            NodoLista* t = cabeza; 
            cabeza = cabeza->siguiente; 
            delete t; 
        }

        cola = nullptr;
    }
};

// ── Lista Doble Circular Sin Encabezado ──
struct ListaDCS {
    NodoLista* cabeza;
    ListaDCS() : cabeza(nullptr) {}

    void insertar(Cancion c) {
        NodoLista* nuevo = new NodoLista{c, nullptr, nullptr};

        if (!cabeza) {
            cabeza = nuevo;
            nuevo->siguiente = nuevo->anterior = nuevo;
            return;
        }

        NodoLista* cola = cabeza->anterior;
        cola->siguiente = nuevo;
        nuevo->anterior = cola;
        nuevo->siguiente = cabeza;
        cabeza->anterior = nuevo;
    }

    NodoLista* siguiente(NodoLista* actual) { 
        return actual ? actual->siguiente : nullptr; 
    }

    NodoLista* anterior(NodoLista* actual)  { 
        return actual ? actual->anterior  : nullptr; 
    }

    void liberar() {
        if (!cabeza) { return; }

        NodoLista* tmp = cabeza->siguiente;
        while (tmp != cabeza) { 
            NodoLista* t = tmp; 
            tmp = tmp->siguiente; 
            delete t; 
        }

        delete cabeza; cabeza = nullptr;
    }
};

// ── Lista Simple Lineal Con Encabezado ──
struct ListaSLCE {
    Encabezado enc;
    NodoLista* cabeza;
    ListaSLCE() : cabeza(nullptr) {}

    void insertar(Cancion c) {
        NodoLista* nuevo = new NodoLista{c, nullptr, nullptr};
        
        if (!cabeza) { 
            cabeza = nuevo; 
            return; 
        }

        NodoLista* tmp = cabeza;
        while (tmp->siguiente) {
            tmp = tmp->siguiente;
        }

        tmp->siguiente = nuevo;
    }

    NodoLista* siguiente(NodoLista* actual) { 
        return actual ? actual->siguiente : nullptr; 
    }

    void liberar() {
        while (cabeza) { 
            NodoLista* t = cabeza; 
            cabeza = cabeza->siguiente; 
            delete t; 
        }
    }
};

// ── Lista Simple Circular Con Encabezado ───
struct ListaSCCE {
    Encabezado enc;
    NodoLista* cabeza;
    ListaSCCE() : cabeza(nullptr) {}

    void insertar(Cancion c) {
        NodoLista* nuevo = new NodoLista{c, nullptr, nullptr};

        if (!cabeza) { 
            cabeza = nuevo; 
            nuevo->siguiente = cabeza; 
            return; 
        }

        NodoLista* tmp = cabeza;
        while (tmp->siguiente != cabeza) {
            tmp = tmp->siguiente;
        }

        tmp->siguiente = nuevo;
        nuevo->siguiente = cabeza;
    }

    NodoLista* siguiente(NodoLista* actual) { 
        return actual ? actual->siguiente : nullptr; 
    }

    void liberar() {
        if (!cabeza) { return; }

        NodoLista* tmp = cabeza->siguiente;
        while (tmp != cabeza) { 
            NodoLista* t = tmp; 
            tmp = tmp->siguiente; 
            delete t; 
        }

        delete cabeza; 
        cabeza = nullptr;
    }
};

// ── Lista Doble Lineal Con Encabezado ──
struct ListaDLCE {
    Encabezado enc;
    NodoLista* cabeza;
    NodoLista* cola;
    ListaDLCE() : cabeza(nullptr), cola(nullptr) {}

    void insertar(Cancion c) {
        NodoLista* nuevo = new NodoLista{c, nullptr, nullptr};
        
        if (!cabeza) { 
            cabeza = cola = nuevo; 
            return; 
        }

        nuevo->anterior = cola;
        cola->siguiente = nuevo;
        cola = nuevo;
    }

    NodoLista* siguiente(NodoLista* actual) { 
        return actual ? actual->siguiente : nullptr; 
    }

    NodoLista* anterior(NodoLista* actual)  { 
        return actual ? actual->anterior  : nullptr; 
    }

    void liberar() {
        while (cabeza) { 
            NodoLista* t = cabeza; 
            cabeza = cabeza->siguiente; 
            delete t; 
        }

        cola = nullptr;
    }
};

// ── Lista Doble Circular Con Encabezado ───
struct ListaDCCE {
    Encabezado enc;
    NodoLista* cabeza;
    ListaDCCE() : cabeza(nullptr) {}

    void insertar(Cancion c) {
        NodoLista* nuevo = new NodoLista{c, nullptr, nullptr};

        if (!cabeza) {
            cabeza = nuevo;
            nuevo->siguiente = nuevo->anterior = nuevo;
            return;
        }

        NodoLista* cola = cabeza->anterior;
        cola->siguiente  = nuevo;
        nuevo->anterior  = cola;
        nuevo->siguiente = cabeza;
        cabeza->anterior = nuevo;
    }

    NodoLista* siguiente(NodoLista* actual) { 
        return actual ? actual->siguiente : nullptr; 
    }

    NodoLista* anterior(NodoLista* actual)  { 
        return actual ? actual->anterior  : nullptr; 
    }

    void liberar() {
        if (!cabeza) { return; }
        NodoLista* tmp = cabeza->siguiente;
        while (tmp != cabeza) { 
            NodoLista* t = tmp; 
            tmp = tmp->siguiente; 
            delete t; 
        }

        delete cabeza; 
        cabeza = nullptr;
    }
};

// ─── Recursos ───
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

void hero() {
    separador();
    cout << CYAN << "  ██╗ █████╗ ███╗   ███╗███╗   ███╗██╗   ██╗███████╗██╗ ██████╗ " << RESET << "\n";
    cout << CYAN << "  ██║██╔══██╗████╗ ████║████╗ ████║██║   ██║██╔════╝██║██╔════╝ " << RESET << "\n";
    cout << CYAN << "  ██║███████║██╔████╔██║██╔████╔██║██║   ██║███████╗██║██║      " << RESET << "\n";
    cout << CYAN << "  ██║██╔══██║██║╚██╔╝██║██║╚██╔╝██║██║   ██║╚════██║██║██║      " << RESET << "\n";
    cout << CYAN << "  ██║██║  ██║██║ ╚═╝ ██║██║ ╚═╝ ██║╚██████╔╝███████║██║╚██████╗" << RESET << "\n";
    cout << CYAN << "  ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝     ╚═╝ ╚═════╝ ╚══════╝╚═╝ ╚═════╝" << RESET << "\n";
    separador();
}

void mostrarReproduciendo() {
    if (hayReproduccion) {
        cout << GREEN << "  ♪  " << cancionActual.titulo
             << " — " << artistaActual
             << " — " << albumActual << RESET << "\n";
        cout << DIM << "  ─────────────────────────────────────────────\n" << RESET;
    }
}

// ─── ARCHIVOS ───

// Artistas | longitud fija (.bin)
struct ArtistaArchivo {
    int id;
    char nombre[50];
    int totalAlbumes;
};

void guardarArtistas() {
    ofstream f("artistas.bin", ios::binary);

    for (int i = 0; i < totalArtistas; i++) {
        ArtistaArchivo a;
        a.id = artistas[i].id;
        strncpy(a.nombre, artistas[i].nombre, 50);
        a.totalAlbumes = artistas[i].totalAlbumes;

        f.write((char*)&a, sizeof(ArtistaArchivo));
    }
    f.close();
}

void cargarArtistas() {
    ifstream f("artistas.bin", ios::binary);
    if (!f) { return; }

    ArtistaArchivo a;
    totalArtistas = 0;
    while (f.read((char*)&a, sizeof(ArtistaArchivo)) && totalArtistas < MAX_ARTISTAS) {
        artistas[totalArtistas].id = a.id;
        strncpy(artistas[totalArtistas].nombre, a.nombre, 50);
        artistas[totalArtistas].totalAlbumes = 0;

        totalArtistas++;
    }
    f.close();
}

// Álbumes | delimitadores (.csv)
void guardarAlbumes() {
    ofstream f("albumes.csv");

    f << "idArtista,idAlbum,titulo,genero,anio,totalCanciones\n";
    for (int i = 0; i < totalArtistas; i++) {
        for (int j = 0; j < artistas[i].totalAlbumes; j++) {
            Album& al = artistas[i].albumes[j];
            f << i << "," << al.id << "," << al.titulo << ","
              << al.genero << "," << al.anio << "," << al.totalCanciones << "\n";
        }
    }
    f.close();
}

void cargarAlbumes() {
    ifstream f("albumes.csv");

    if (!f) { return; }
    string linea;
    getline(f, linea); // saltar encabezado
    
    while (getline(f, linea)) {
        int idArtista, idAlbum, anio, totalCanciones;
        char titulo[50], genero[30];
        sscanf(linea.c_str(), "%d,%d,%49[^,],%29[^,],%d,%d",
               &idArtista, &idAlbum, titulo, genero, &anio, &totalCanciones);
        if (idArtista < totalArtistas) {
            int j = artistas[idArtista].totalAlbumes;

            artistas[idArtista].albumes[j].id = idAlbum;
            strncpy(artistas[idArtista].albumes[j].titulo, titulo, 50);
            strncpy(artistas[idArtista].albumes[j].genero, genero, 30);
            artistas[idArtista].albumes[j].anio = anio;
            artistas[idArtista].albumes[j].totalCanciones = 0;

            artistas[idArtista].totalAlbumes++;
        }
    }
    f.close();
}

// Canciones | campos dimensionales (.txt)
void guardarCanciones() {
    ofstream f("canciones.txt");

    for (int i = 0; i < totalArtistas; i++) {
        for (int j = 0; j < artistas[i].totalAlbumes; j++) {
            for (int k = 0; k < artistas[i].albumes[j].totalCanciones; k++) {
                Cancion& c = artistas[i].albumes[j].canciones[k];

                int lenTitulo = strlen(c.titulo);
                f << i << " " << j << " " << c.id << " "
                  << lenTitulo << " " << c.titulo << " " << c.numeroPista << "\n";
            }
        }
    }
    f.close();
}

void cargarCanciones() {
    ifstream f("canciones.txt");

    if (!f) { return; }
    int idArtista, idAlbum, id, lenTitulo, pista;
    char titulo[50];

    while (f >> idArtista >> idAlbum >> id >> lenTitulo) {
        f.ignore();
        f.read(titulo, lenTitulo);
        titulo[lenTitulo] = '\0';
        f >> pista;

        if (idArtista < totalArtistas && idAlbum < artistas[idArtista].totalAlbumes) {
            int k = artistas[idArtista].albumes[idAlbum].totalCanciones;

            artistas[idArtista].albumes[idAlbum].canciones[k].id = id;
            strncpy(artistas[idArtista].albumes[idAlbum].canciones[k].titulo, titulo, 50);
            artistas[idArtista].albumes[idAlbum].canciones[k].numeroPista = pista;

            artistas[idArtista].albumes[idAlbum].totalCanciones++;
        }
    }
    f.close();
}

void guardarTodo() {
    guardarArtistas();
    guardarAlbumes();
    guardarCanciones();
    cout << GREEN << "\n Datos guardados correctamente.\n" << RESET;
}

void cargarTodo() {
    cargarArtistas();
    cargarAlbumes();
    cargarCanciones();
    cout << GREEN << "\n Datos cargados correctamente.\n" << RESET;
}

// ─── REGISTRO ───
void registrarArtista() {
    if (totalArtistas >= MAX_ARTISTAS) {
        cout << RED << "\n  Límite de artistas alcanzado.\n" << RESET;
        pausar(); 
        return;
    }

    cin.ignore();
    cout << "\n  Nombre del artista: ";
    cin.getline(artistas[totalArtistas].nombre, 50);
    artistas[totalArtistas].id = totalArtistas + 1;
    artistas[totalArtistas].totalAlbumes = 0;

    totalArtistas++;
    cout << GREEN << "\n  ✓ Artista registrado.\n" << RESET;
    pausar();
}

void registrarAlbum() {
    if (totalArtistas == 0) {
        cout << RED << "\n  ⚠ No hay artistas registrados. Registra uno primero.\n" << RESET;
        
        pausar(); 
        return;
    }

    cout << "\n  Selecciona artista:\n";
    for (int i = 0; i < totalArtistas; i++) {
        cout << "  [" << i+1 << "] " << artistas[i].nombre << "\n";
    }

    cout << GRAY << "  >> " << RESET;
    int sel; cin >> sel; sel--;
    if (sel < 0 || sel >= totalArtistas) { 
        cout << RED << "\n  Opción inválida.\n" << RESET; pausar(); 
        return; 
    }

    if (artistas[sel].totalAlbumes >= 10) { 
        cout << RED << "\n  Límite de álbumes alcanzado.\n" << RESET; 
        pausar(); 
        return; 
    }

    int j = artistas[sel].totalAlbumes;
    cin.ignore();
    cout << "  Título del álbum: ";
    cin.getline(artistas[sel].albumes[j].titulo, 50);
    cout << "  Género: ";
    cin.getline(artistas[sel].albumes[j].genero, 30);
    cout << "  Año: ";
    cin >> artistas[sel].albumes[j].anio;
    artistas[sel].albumes[j].id = j + 1;
    artistas[sel].albumes[j].totalCanciones = 0;
    artistas[sel].totalAlbumes++;
    cout << GREEN << "\n Álbum registrado.\n" << RESET;
    pausar();
}

void registrarCancion() {
    if (totalArtistas == 0) {
        cout << RED << "\n No hay artistas registrados.\n" << RESET;
        pausar(); 
        return;
    }

    cout << "\n  Selecciona artista:\n";
    for (int i = 0; i < totalArtistas; i++) {
        cout << "  [" << i+1 << "] " << artistas[i].nombre << "\n";
    }
    cout << GRAY << "  >> " << RESET;

    int selA; cin >> selA; selA--;
    if (selA < 0 || selA >= totalArtistas) { 
        cout << RED << "\n  Opción inválida.\n" << RESET; 
        pausar(); 
        return; 
    }

    if (artistas[selA].totalAlbumes == 0) {
        cout << RED << "\n  ⚠ Este artista no tiene álbumes.\n" << RESET;
        pausar(); 
        return;
    }

    cout << "  Selecciona álbum:\n";
    for (int j = 0; j < artistas[selA].totalAlbumes; j++) {
        cout << "  [" << j+1 << "] " << artistas[selA].albumes[j].titulo << "\n";
    }
    cout << GRAY << "  >> " << RESET;

    int selB; cin >> selB; selB--;
    if (selB < 0 || selB >= artistas[selA].totalAlbumes) { 
        cout << RED << "\n  Opción inválida.\n" << RESET; 
        pausar(); 
        return; 
    }

    if (artistas[selA].albumes[selB].totalCanciones >= 10) { 
        cout << RED << "\n  Límite de canciones alcanzado.\n" << RESET; pausar(); 
        return; 
    }

    int k = artistas[selA].albumes[selB].totalCanciones;
    cin.ignore();
    cout << "  Título de la canción: ";
    cin.getline(artistas[selA].albumes[selB].canciones[k].titulo, 50);
    cout << "  Número de pista: ";
    cin >> artistas[selA].albumes[selB].canciones[k].numeroPista;
    artistas[selA].albumes[selB].canciones[k].id = k + 1;

    artistas[selA].albumes[selB].totalCanciones++;
    cout << GREEN << "\n  ✓ Canción registrada.\n" << RESET;

    pausar();
}

void eliminarArtista() {
    if (totalArtistas == 0) {
        cout << RED << "\n  No hay artistas registrados.\n" << RESET;
        pausar();
        return;
    }

    cout << "\n  Selecciona artista a eliminar:\n";
    for (int i = 0; i < totalArtistas; i++) {
        cout << "  [" << i+1 << "] " << artistas[i].nombre << "\n";
    }
    cout << GRAY << "  >> " << RESET;

    int sel; 
    cin >> sel; 
    sel--;

    if (sel < 0 || sel >= totalArtistas) { 
        cout << RED << "\n  Opción inválida.\n" << RESET; 
        pausar(); 
        return; 
    }

    for (int i = sel; i < totalArtistas - 1; i++) {
        artistas[i] = artistas[i+1];
    }

    totalArtistas--;
    cout << GREEN << "\n  ✓ Artista eliminado.\n" << RESET;
    pausar();
}

// ─── PANTALLA PRINCIPAL ───
// cargar todas las canciones de todos los artistas/álbumes
void cargarTodasEnCola() {
    while (frenteCola) {
        NodoCola* temp = frenteCola;
        frenteCola = frenteCola->siguiente;
        delete temp;
    }
    finalCola = nullptr;

    for (int i = 0; i < totalArtistas; i++) {
        for (int j = 0; j < artistas[i].totalAlbumes; j++) {
            for (int k = 0; k < artistas[i].albumes[j].totalCanciones; k++) {
                enqueueCola(artistas[i].albumes[j].canciones[k],
                            artistas[i].nombre,
                            artistas[i].albumes[j].titulo);
            }
        }
    }
}

void mostrarTodasCanciones() {
    int num = 1;
    bool hay = false;
    for (int i = 0; i < totalArtistas; i++) {
        for (int j = 0; j < artistas[i].totalAlbumes; j++) {
            for (int k = 0; k < artistas[i].albumes[j].totalCanciones; k++) {
                hay = true;
                bool esActual = hayReproduccion && strcmp(cancionActual.titulo,
                     artistas[i].albumes[j].canciones[k].titulo) == 0;

                if (esActual) {
                    cout << GREEN << "  ♪ ";
                } else {
                    cout << GRAY  << "  " << num << ". ";
                }

                cout << WHITE << artistas[i].albumes[j].canciones[k].titulo
                     << GRAY << " — " << artistas[i].nombre
                     << " — " << artistas[i].albumes[j].titulo
                     << RESET << "\n";
                num++;
            }
        }
    }
    if (!hay) {
        cout << GRAY << "\n  No hay canciones registradas.\n"
             << "  Ve a Registro para agregar artistas, álbumes y canciones.\n" << RESET;
    }
}

// ─── MENÚ REGISTRO ───
void menuRegistro() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        hero();
        mostrarReproduciendo();
        cout << "\n  " << CYAN << "REGISTRO\n\n" << RESET;
        cout << "  " << CYAN << "[1]" << RESET << WHITE << "  Nuevo artista\n"  << RESET;
        cout << "  " << CYAN << "[2]" << RESET << WHITE << "  Nuevo álbum\n"    << RESET;
        cout << "  " << CYAN << "[3]" << RESET << WHITE << "  Nueva canción\n"  << RESET;
        cout << "  " << RED  << "[4]" << RESET << RED   << "  Eliminar artista\n" << RESET;
        cout << "\n  " << DIM << "[0]" << RESET << DIM  << "  Regresar\n" << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: registrarArtista(); break;
            case 2: registrarAlbum();   break;
            case 3: registrarCancion(); break;
            case 4: eliminarArtista();  break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

// ─── BÚSQUEDAS ───

// Pone a los álbumes en un arreglo ordenar y buscar
struct AlbumPlano {
    int idArtista;
    int idAlbum;
    char titulo[50];
    char genero[30];
    int anio;
    int totalCanciones;
};

int recolectarAlbumes(AlbumPlano lista[], int maxLista) {
    int total = 0;
    for (int i = 0; i < totalArtistas && total < maxLista; i++) {
        for (int j = 0; j < artistas[i].totalAlbumes && total < maxLista; j++) {
            lista[total].idArtista = i;
            lista[total].idAlbum = j;
            strncpy(lista[total].titulo, artistas[i].albumes[j].titulo, 50);
            strncpy(lista[total].genero, artistas[i].albumes[j].genero, 30);
            lista[total].anio = artistas[i].albumes[j].anio;
            lista[total].totalCanciones = artistas[i].albumes[j].totalCanciones;

            total++;
        }
    }
    return total;
}

// Bubble Sort por año del más reciente al más viejo
void bubbleSortAnio(AlbumPlano lista[], int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (lista[j].anio < lista[j+1].anio) {
                AlbumPlano temp = lista[j];
                lista[j] = lista[j+1];
                lista[j+1] = temp;
            }
        }
    }
}

// Búsqueda binaria por año
int busquedaBinariaAnio(AlbumPlano lista[], int n, int anio) {
    int izq = 0;
    int der = n - 1;

    while (izq <= der) {
        int mid = (izq + der) / 2;

        if (lista[mid].anio == anio) { 
            return mid;
        } else if (lista[mid].anio > anio) {
            izq = mid + 1;
        } else {
            der = mid - 1;
        }
    }
    return -1;
}

// Búsqueda lineal por título
int busquedaLinealArreglo(AlbumPlano lista[], int n, const char* titulo) {
    for (int i = 0; i < n; i++) {
        if (strstr(lista[i].titulo, titulo) != nullptr) {
            return i;
        }
    }
    return -1;
}

// Búsqueda lineal por título en .CSV
void busquedaLinealCSV(const char* titulo) {
    ifstream f("albumes.csv");

    if (!f) { 
        cout << RED << "\n  ⚠ No se encontró albumes.csv\n" << RESET; 
        return; 
    }

    string linea;
    getline(f, linea); // encabezado
    bool encontrado = false;
    int lineaNum = 0;

    while (getline(f, linea)) {
        lineaNum++;
        
        if (linea.find(titulo) != string::npos) {
            cout << GREEN << "\n  ✓ Encontrado en línea " << lineaNum << ": " << linea << RESET << "\n";
            encontrado = true;
        }
    }

    if (!encontrado) {
        cout << RED << "\n  No se encontró \"" << titulo << "\" en el archivo.\n" << RESET;
    }
    f.close();
}

// Carga el álbum seleccionado según la lista
void cargarAlbumEnCola(int idArtista, int idAlbum) {
    while (frenteCola) {
        NodoCola* temp = frenteCola;
        frenteCola = frenteCola->siguiente;
        delete temp;
    }
    finalCola = nullptr;

    Album& al = artistas[idArtista].albumes[idAlbum];
    for (int k = 0; k < al.totalCanciones; k++) {
        enqueueCola(al.canciones[k], artistas[idArtista].nombre, al.titulo);
    }
}

void seleccionarTipoListaYReproducir(int idArtista, int idAlbum) {
    limpiarPantalla();
    hero();

    Album& al = artistas[idArtista].albumes[idAlbum];
    cout << "\n  " << CYAN << "Álbum: " << WHITE << al.titulo
         << CYAN << "  |  Artista: " << WHITE << artistas[idArtista].nombre
         << CYAN << "  |  Año: " << WHITE << al.anio << RESET << "\n\n";
    cout << "  Selecciona el tipo de lista para reproducir:\n\n";
    cout << "  " << CYAN << "[1]" << RESET << "  Lista Simple Lineal Sin Encabezado\n";
    cout << "  " << CYAN << "[2]" << RESET << "  Lista Simple Circular Sin Encabezado\n";
    cout << "  " << CYAN << "[3]" << RESET << "  Lista Doble Lineal Sin Encabezado\n";
    cout << "  " << CYAN << "[4]" << RESET << "  Lista Doble Circular Sin Encabezado\n";
    cout << "  " << CYAN << "[5]" << RESET << "  Lista Simple Lineal Con Encabezado\n";
    cout << "  " << CYAN << "[6]" << RESET << "  Lista Simple Circular Con Encabezado\n";
    cout << "  " << CYAN << "[7]" << RESET << "  Lista Doble Lineal Con Encabezado\n";
    cout << "  " << CYAN << "[8]" << RESET << "  Lista Doble Circular Con Encabezado\n";
    cout << "\n"; separador();
    cout << GRAY << "  >> " << RESET;

    int tipoLista; cin >> tipoLista;
    if (tipoLista < 1 || tipoLista > 8) {
        cout << RED << "\n  Opción inválida.\n" << RESET; 
        pausar(); 
        return;
    }

    string nomArtista = artistas[idArtista].nombre;
    string nomAlbum = al.titulo;
    bool esDoble = (tipoLista == 3 || tipoLista == 4 || tipoLista == 7 || tipoLista == 8);
    bool esCircular = (tipoLista == 2 || tipoLista == 4 || tipoLista == 6 || tipoLista == 8);
    bool tieneEnc = (tipoLista >= 5);

    // con encabezado
    Encabezado enc;
    strncpy(enc.titulo, al.titulo,  50);
    strncpy(enc.artista, artistas[idArtista].nombre, 50);
    strncpy(enc.genero,al.genero,  30);
    enc.anio = al.anio;
    enc.totalCanciones = al.totalCanciones;

    ListaSLS l1; 
    ListaSCS l2; 
    ListaDLS l3; 
    ListaDCS l4;
    ListaSLCE l5; 
    ListaSCCE l6; 
    ListaDLCE l7; 
    ListaDCCE l8;

    for (int k = 0; k < al.totalCanciones; k++) {
        switch(tipoLista) {
            case 1: l1.insertar(al.canciones[k]); break;
            case 2: l2.insertar(al.canciones[k]); break;
            case 3: l3.insertar(al.canciones[k]); break;
            case 4: l4.insertar(al.canciones[k]); break;
            case 5: l5.insertar(al.canciones[k]); break;
            case 6: l6.insertar(al.canciones[k]); break;
            case 7: l7.insertar(al.canciones[k]); break;
            case 8: l8.insertar(al.canciones[k]); break;
        }
    }

    if (tieneEnc) {
        if (tipoLista == 5) {
            l5.enc = enc; 
            if (tipoLista == 6) {
                l6.enc = enc;
            }
        }

        if (tipoLista == 7) {
            l7.enc = enc; 
            if (tipoLista == 8) {
                l8.enc = enc;
            }
        }
    }

    NodoLista* actual = nullptr;
    switch(tipoLista) {
        case 1: actual = l1.cabeza; break; 
        case 2: actual = l2.cabeza; break;
        case 3: actual = l3.cabeza; break; 
        case 4: actual = l4.cabeza; break;
        case 5: actual = l5.cabeza; break; 
        case 6: actual = l6.cabeza; break;
        case 7: actual = l7.cabeza; break; 
        case 8: actual = l8.cabeza; break;
    }

    if (!actual) {
        cout << RED << "\n  El álbum no tiene canciones.\n" << RESET;
        pausar(); 
        return;
    }

    // ── Reproductor por lista ───
    char op2[10];
    bool salir = false;

    while (!salir) {
        limpiarPantalla();
        hero();

        // mostrar encabezado
        if (tieneEnc) {
            cout << CYAN << "  Album : " << WHITE << enc.titulo
                 << CYAN << "  |  Artista: " << WHITE << enc.artista
                 << CYAN << "  |  Año: " << WHITE << enc.anio
                 << CYAN << "  |  Género: " << WHITE << enc.genero
                 << CYAN << "  |  Canciones: " << WHITE << enc.totalCanciones
                 << RESET << "\n";
            cout << DIM << "  ─────────────────────────────────────────────\n" << RESET;
        }

        // Canción actual
        cout << GREEN << "\n  ♪  " << actual->cancion.titulo
             << "  " << DIM << "(pista " << actual->cancion.numeroPista << ")"
             << RESET << "\n";
        cout << GRAY << "  " << nomArtista << " — " << nomAlbum << RESET << "\n\n";

        // Tipo de lista
        const char* nombres[] = {"","Simple Lineal S/E","Simple Circular S/E",
            "Doble Lineal S/E","Doble Circular S/E","Simple Lineal C/E",
            "Simple Circular C/E","Doble Lineal C/E","Doble Circular C/E"};
        cout << DIM << "  Lista: " << nombres[tipoLista] << RESET << "\n\n";

        separador();
        
        cout << "  " << CYAN << "[S]" << RESET << WHITE << "  Siguiente  ⏭\n" << RESET;
        if (esDoble) {
            cout << "  " << CYAN << "[A]" << RESET << WHITE << "  Anterior   ⏮\n" << RESET;
        } else {
            cout << "  " << DIM  << "[A]" << RESET << DIM   << "  Anterior   ⏮  (no disponible en lista simple)\n" << RESET;
        }
        
        cout << "\n  " << RED << "[0]" << RESET << RED << "  Salir\n" << RESET;
        separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op2;

        if (op2[0] == 'S' || op2[0] == 's') {
            NodoLista* sig = nullptr;
            switch(tipoLista) {
                case 1: sig = l1.siguiente(actual); break;
                case 2: sig = l2.siguiente(actual); break;
                case 3: sig = l3.siguiente(actual); break;
                case 4: sig = l4.siguiente(actual); break;
                case 5: sig = l5.siguiente(actual); break;
                case 6: sig = l6.siguiente(actual); break;
                case 7: sig = l7.siguiente(actual); break;
                case 8: sig = l8.siguiente(actual); break;
            }

            if (!esCircular && !sig) {
                cout << GRAY << "\n  Fin del álbum.\n" << RESET;
                pausar();
            } else {
                actual = sig;
            }

        } else if (op2[0] == 'A' || op2[0] == 'a') {
            if (!esDoble) {
                cout << RED << "\n  No disponible en lista simple.\n" << RESET;
                pausar();
            } else {
                NodoLista* ant = nullptr;
                switch(tipoLista) {
                    case 3: ant = l3.anterior(actual); break;
                    case 4: ant = l4.anterior(actual); break;
                    case 7: ant = l7.anterior(actual); break;
                    case 8: ant = l8.anterior(actual); break;
                }

                if (!esCircular && !ant) {
                    cout << GRAY << "\n  Ya estás en la primera canción.\n" << RESET;
                    pausar();
                } else {
                    actual = ant;
                }
            }
        } else if (atoi(op2) == 0) {
            salir = true;
        }
    }

    // Limpiar listas
    l1.liberar(); 
    l2.liberar(); 
    l3.liberar(); 
    l4.liberar();
    l5.liberar(); 
    l6.liberar(); 
    l7.liberar(); 
    l8.liberar();

    // Limpiar reproducción
    hayReproduccion = false;
    artistaActual = "";
    albumActual = "";
}

void mostrarResultadosYSeleccionar(AlbumPlano lista[], int n) {
    if (n == 0) { 
        cout << RED << "\n  No se encontraron resultados.\n" << RESET; 
        pausar(); 
        return; 
    }

    cout << "\n  Resultados:\n\n";
    for (int i = 0; i < n; i++) {
        cout << "  [" << i+1 << "] " << WHITE << lista[i].titulo
             << GRAY << " — " << lista[i].anio
             << " — " << lista[i].genero << RESET << "\n";
    }

    cout << "\n  Selecciona un álbum para reproducir (0 = cancelar): ";
    cout << "\n"; separador();
    cout << GRAY << "  >> " << RESET;

    int sel; cin >> sel;
    if (sel < 1 || sel > n) { return; }
    sel--;
    seleccionarTipoListaYReproducir(lista[sel].idArtista, lista[sel].idAlbum);
}

// ─── ÁRBOL ───
void explorarArbol() {
    limpiarPantalla();
    hero();
    mostrarReproduciendo();
    cout << "\n  " << CYAN << "ÁRBOL DE BIBLIOTECA\n\n" << RESET;

    if (totalArtistas == 0) {
        cout << GRAY << "  No hay artistas registrados.\n" << RESET;
        pausar(); 
        return;
    }

    // Imprimir árbol
    for (int i = 0; i < totalArtistas; i++) {
        cout << CYAN << "  [" << i+1 << "] " << WHITE << artistas[i].nombre << RESET << "\n";
        
        for (int j = 0; j < artistas[i].totalAlbumes; j++) {
            cout << GRAY << "      ├── " << YELLOW
                 << artistas[i].albumes[j].titulo
                 << DIM << "  (" << artistas[i].albumes[j].anio
                 << " — " << artistas[i].albumes[j].genero << ")"
                 << RESET << "\n";

            for (int k = 0; k < artistas[i].albumes[j].totalCanciones; k++) {
                bool esUltima = (k == artistas[i].albumes[j].totalCanciones - 1);
                cout << GRAY << "      │     " << (esUltima ? "└── " : "├── ")
                     << WHITE << artistas[i].albumes[j].canciones[k].titulo
                     << DIM << "  (pista " << artistas[i].albumes[j].canciones[k].numeroPista << ")"
                     << RESET << "\n";
            }
        }
        cout << "\n";
    }

    separador();
    cout << "  Selecciona artista para reproducir un álbum (0 = cancelar):\n";
    cout << GRAY << "  >> " << RESET;
    int selA; cin >> selA;
    if (selA < 1 || selA > totalArtistas) { return; }
    selA--;

    if (artistas[selA].totalAlbumes == 0) {
        cout << RED << "\n  Este artista no tiene álbumes.\n" << RESET;
        pausar(); 
        return;
    }

    cout << "\n  Selecciona álbum:\n";
    for (int j = 0; j < artistas[selA].totalAlbumes; j++) {
        cout << "  [" << j+1 << "] " << artistas[selA].albumes[j].titulo << "\n";
    }

    cout << GRAY << "  >> " << RESET;
    int selB; cin >> selB;

    if (selB < 1 || selB > artistas[selA].totalAlbumes)  {
        return;
    }

    selB--;
    seleccionarTipoListaYReproducir(selA, selB);
}

// ─── GÉNEROS — Matriz y lista de adyacencia ────
const int MAX_GENEROS = 20;
char generos[MAX_GENEROS][30];
int totalGeneros = 0;
int matrizAdj[MAX_GENEROS][MAX_GENEROS];

// Lee géneros de álbumes registrados
void extraerGeneros() {
    totalGeneros = 0;
    memset(matrizAdj, 0, sizeof(matrizAdj));

    for (int i = 0; i < totalArtistas; i++) {
        for (int j = 0; j < artistas[i].totalAlbumes; j++) {
            const char* g = artistas[i].albumes[j].genero;
            bool existe = false;

            for (int k = 0; k < totalGeneros; k++) {
                if (strcmp(generos[k], g) == 0) { 
                    existe = true; 
                    break; 
                }
            }

            if (!existe && totalGeneros < MAX_GENEROS) {
                strncpy(generos[totalGeneros++], g, 30);
            }
        }
    }
}

int indiceGenero(const char* g) {
    for (int k = 0; k < totalGeneros; k++) {
        if (strcmp(generos[k], g) == 0) {
            return k;
        }
    }

    return -1;
}

void menuGeneros() {
    extraerGeneros();
    int op = -1;

    while (op != 0) {
        limpiarPantalla();
        hero();
        mostrarReproduciendo();

        cout << "\n  " << CYAN << "GÉNEROS RELACIONADOS\n\n" << RESET;

        if (totalGeneros == 0) {
            cout << GRAY << "  No hay géneros registrados aún.\n"
                 << "  Registra álbumes primero.\n" << RESET;
            pausar(); 
            return;
        }

        cout << "  " << CYAN << "[1]" << RESET << WHITE << "  Registrar relación entre géneros\n" << RESET;
        cout << "  " << CYAN << "[2]" << RESET << WHITE << "  Ver matriz de adyacencia\n"          << RESET;
        cout << "  " << CYAN << "[3]" << RESET << WHITE << "  Ver lista de adyacencia\n"           << RESET;
        cout << "  " << CYAN << "[4]" << RESET << WHITE << "  Ver recomendaciones\n"               << RESET;
        cout << "\n  " << DIM << "[0]" << RESET << DIM  << "  Regresar\n"                         << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        switch (op) {
            case 1: {
                // registro
                limpiarPantalla(); 
                hero();
                cout << "\n  " << CYAN << "Géneros disponibles:\n\n" << RESET;

                for (int k = 0; k < totalGeneros; k++) {
                    cout << "  [" << k+1 << "] " << generos[k] << "\n";
                }

                cout << "\n  Selecciona género A: ";
                int a; 
                cin >> a; 
                a--;

                cout << "  Selecciona género B: ";
                int b; 
                cin >> b; 
                b--;

                if (a < 0 || a >= totalGeneros || b < 0 || b >= totalGeneros || a == b) {
                    cout << RED << "\n  Selección inválida.\n" << RESET;
                } else {
                    matrizAdj[a][b] = 1;
                    matrizAdj[b][a] = 1;
                    cout << GREEN << "\n  ✓ Relación registrada: "
                         << generos[a] << " ↔ " << generos[b] << RESET << "\n";
                }

                pausar(); 
                break;
            }

            case 2: {
                // Matriz de adyacencia
                limpiarPantalla(); 
                hero();
                cout << "\n  " << CYAN << "MATRIZ DE ADYACENCIA\n\n" << RESET;
                cout << "\t\t   ";

                for (int k = 0; k < totalGeneros; k++) {
                    cout << DIM << "[" << k+1 << "]  " << RESET;
                }

                cout << "\n";
                for (int i = 0; i < totalGeneros; i++) {
                    cout << "  " << YELLOW << "[" << i+1 << "] " << WHITE << generos[i] << RESET;
                    int pad = 12 - strlen(generos[i]);

                    for (int p = 0; p < pad; p++) {
                        cout << " ";
                    }

                    for (int j = 0; j < totalGeneros; j++) {
                        if (matrizAdj[i][j]) {
                            cout << GREEN << "  1  " << RESET;
                        } else {
                            cout << DIM   << "  0  " << RESET;
                        }
                    }

                    cout << "\n";
                }
                pausar(); 
                break;
            }

            case 3: {
                // Lista de adyacencia
                limpiarPantalla(); 
                hero();
                cout << "\n  " << CYAN << "LISTA DE ADYACENCIA\n\n" << RESET;

                for (int i = 0; i < totalGeneros; i++) {
                    cout << "  " << YELLOW << generos[i] << RESET;
                    bool tieneVecinos = false;

                    for (int j = 0; j < totalGeneros; j++) {
                        if (matrizAdj[i][j]) {
                            cout << GRAY << " → " << WHITE << generos[j] << RESET;
                            tieneVecinos = true;
                        }
                    }

                    if (!tieneVecinos) {
                        cout << DIM << "  (sin relaciones)" << RESET;
                    }
                    cout << "\n";
                }

                pausar(); 
                break;
            }

            case 4: {
                // Recomendaciones basadas en canción actual
                limpiarPantalla(); 
                hero();
                cout << "\n  " << CYAN << "RECOMENDACIONES\n\n" << RESET;
                
                if (!hayReproduccion) {
                    cout << GRAY << "  Reproduce una canción primero.\n" << RESET;
                    pausar(); break;
                }

                int idxGeneroActual = -1;
                for (int i = 0; i < totalArtistas && idxGeneroActual == -1; i++) {
                    for (int j = 0; j < artistas[i].totalAlbumes && idxGeneroActual == -1; j++) {
                        if (strcmp(artistas[i].albumes[j].titulo, albumActual.c_str()) == 0) {
                            idxGeneroActual = indiceGenero(artistas[i].albumes[j].genero);
                        }
                    }
                }

                if (idxGeneroActual == -1) {
                    cout << GRAY << "  No se encontró el género de la canción actual.\n" << RESET;
                    pausar(); 
                    break;
                }

                cout << "  Escuchando: " << GREEN << cancionActual.titulo
                     << RESET << " — " << YELLOW << generos[idxGeneroActual] << RESET << "\n\n";

                bool hayRec = false;
                for (int j = 0; j < totalGeneros; j++) {
                    if (matrizAdj[idxGeneroActual][j]) {
                        cout << "  " << CYAN << "Género relacionado: " << WHITE << generos[j] << RESET << "\n";
                        
                        for (int i = 0; i < totalArtistas; i++) {
                            for (int k = 0; k < artistas[i].totalAlbumes; k++) {
                                if (strcmp(artistas[i].albumes[k].genero, generos[j]) == 0) {
                                    cout << GRAY << "    → " << WHITE << artistas[i].albumes[k].titulo
                                         << " — " << artistas[i].nombre << RESET << "\n";
                                }
                            }
                        }

                        hayRec = true;
                    }
                }

                if (!hayRec) {
                    cout << GRAY << "  No hay géneros relacionados definidos.\n"
                         << "  Ve a [1] para registrar relaciones.\n" << RESET;
                }

                pausar();
                break;
            }

            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

// ─── MENÚ BUSCAR ───
void menuBuscar() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        hero();
        mostrarReproduciendo();
        cout << "\n  " << CYAN << "BUSCAR\n\n" << RESET;
        cout << "  " << CYAN << "[1]" << RESET << WHITE << "  Ordenar álbumes por año\n"           << RESET;
        cout << "  " << CYAN << "[2]" << RESET << WHITE << "  Buscar álbum por año  " << DIM << "(binaria)\n"  << RESET;
        cout << "  " << CYAN << "[3]" << RESET << WHITE << "  Buscar álbum por título  " << DIM << "(lineal en arreglo)\n" << RESET;
        cout << "  " << CYAN << "[4]" << RESET << WHITE << "  Buscar álbum por título  " << DIM << "(lineal en CSV)\n"     << RESET;
        cout << "  " << CYAN << "[5]" << RESET << WHITE << "  Explorar árbol de biblioteca\n"                               << RESET;
        cout << "\n  " << DIM << "[0]" << RESET << DIM  << "  Regresar\n" << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;

        AlbumPlano lista[50];
        int total = recolectarAlbumes(lista, 50);

        switch (op) {
            case 1: {
                bubbleSortAnio(lista, total);
                mostrarResultadosYSeleccionar(lista, total);
                break;
            }

            case 2: {
                bubbleSortAnio(lista, total);
                cout << "\n  Ingresa el año a buscar: ";
                int anio; cin >> anio;
                int idx = busquedaBinariaAnio(lista, total, anio);

                if (idx == -1) {
                    cout << RED << "\n  No se encontró ningún álbum del año " << anio << "\n" << RESET;
                    pausar();
                } else {
                    AlbumPlano resultados[50];
                    int nRes = 0;

                    for (int i = 0; i < total; i++) {
                        if (lista[i].anio == anio) {
                            resultados[nRes++] = lista[i];
                        }
                    }
                    mostrarResultadosYSeleccionar(resultados, nRes);
                }
                break;
            }

            case 3: {
                cin.ignore();
                cout << "\n  Título a buscar: ";
                char titulo[50]; cin.getline(titulo, 50);
                AlbumPlano resultados[50]; int nRes = 0;

                for (int i = 0; i < total; i++) {
                    if (strstr(lista[i].titulo, titulo) != nullptr) {
                        resultados[nRes++] = lista[i];
                    }
                }

                mostrarResultadosYSeleccionar(resultados, nRes);
                break;
            }

            case 4: {
                cin.ignore();
                cout << "\n  Título a buscar en CSV: ";
                char titulo[50]; cin.getline(titulo, 50);
                busquedaLinealCSV(titulo);
                pausar();
                break;
            }

            case 5: explorarArbol(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

// ─── MENÚ ARCHIVOS ────
void menuArchivos() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        hero();
        mostrarReproduciendo();
        cout << "\n  " << CYAN << "ARCHIVOS\n\n" << RESET;
        cout << "  " << CYAN << "[1]" << RESET << WHITE << "  Guardar datos\n" << RESET;
        cout << DIM  << "        artistas.bin  |  albumes.csv  |  canciones.txt\n" << RESET;
        cout << "\n  " << CYAN << "[2]" << RESET << WHITE << "  Cargar datos\n" << RESET;
        cout << DIM  << "        artistas.bin  |  albumes.csv  |  canciones.txt\n" << RESET;
        cout << "\n  " << DIM << "[0]" << RESET << DIM << "  Regresar\n" << RESET;
        cout << "\n"; separador();
        cout << GRAY << "  >> " << RESET;
        cin >> op;
        switch (op) {
            case 1: guardarTodo(); pausar(); break;
            case 2: cargarTodo(); pausar(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

// ─── PANTALLA PRINCIPAL ───
void pantallaPrincipal() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        hero();

        mostrarReproduciendo();
        cout << "\n";
        mostrarTodasCanciones();

        cout << "\n"; separador();
        cout << "  " << CYAN << "[R]" << RESET << WHITE << "  Reproducir desde el inicio\n" << RESET;
        cout << "  " << CYAN << "[S]" << RESET << WHITE << "  Siguiente canción  ⏭\n" << RESET;
        cout << "  " << CYAN << "[A]" << RESET << WHITE << "  Canción anterior   ⏮\n" << RESET;
        cout << DIM  << "  ─────────────────────────────────────────────\n" << RESET;
        cout << "  " << CYAN << "[1]" << RESET << WHITE << "  Registro\n" << RESET;
        cout << "  " << CYAN << "[2]" << RESET << WHITE << "  Buscar álbum\n" << RESET;
        cout << "  " << CYAN << "[3]" << RESET << WHITE << "  Archivos\n" << RESET;
        cout << "  " << CYAN << "[4]" << RESET << WHITE << "  Géneros relacionados\n" << RESET;
        cout << "\n  " << RED << "[0]" << RESET << RED  << "  Salir\n" << RESET;
        separador();
        cout << GRAY << "  >> " << RESET;

        char entrada[10];
        cin >> entrada;

        if (entrada[0] == 'R' || entrada[0] == 'r') {
            cargarTodasEnCola();
            if (dequeueCola(cancionActual, artistaActual, albumActual)) {
                pushPila(cancionActual, artistaActual, albumActual);
                hayReproduccion = true;
                cout << GREEN << "\n  ♪ Reproduciendo: " << cancionActual.titulo << RESET << "\n";
            } else {
                cout << RED << "\n  No hay canciones para reproducir.\n" << RESET;
            }
            pausar();

        } else if (entrada[0] == 'S' || entrada[0] == 's') {
            Cancion c; string art, alb;

            if (dequeueCola(c, art, alb)) {
                if (hayReproduccion) {
                    pushPila(cancionActual, artistaActual, albumActual);
                }

                cancionActual = c; artistaActual = art; albumActual = alb;
                hayReproduccion = true;
                cout << GREEN << "\n  ♪ Siguiente: " << cancionActual.titulo << RESET << "\n";
            } else {
                cout << GRAY << "\n  No hay más canciones en la cola.\n" << RESET;
            }
            pausar();

        } else if (entrada[0] == 'A' || entrada[0] == 'a') {
            Cancion c; string art, alb;

            if (popPila(c, art, alb)) {
                if (hayReproduccion) {
                    enqueueCola(cancionActual, artistaActual, albumActual);
                }

                cancionActual = c; artistaActual = art; albumActual = alb;
                hayReproduccion = true;
                cout << GREEN << "\n  ♪ Anterior: " << cancionActual.titulo << RESET << "\n";
            } else {
                cout << GRAY << "\n  No hay canciones anteriores.\n" << RESET;
            }
            pausar();

        } else {
            op = atoi(entrada);
            switch (op) {
                case 1: menuRegistro(); break;
                case 2: menuBuscar();   break;
                case 3: menuArchivos(); break;
                case 4: menuGeneros();  break;
                case 0: break;
                default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar(); op = -1;
            }
        }
    }
}

// ─── MAIN ───
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    // intentar cargar datos al inicio
    cargarTodo();

    pantallaPrincipal();
    limpiarPantalla();
    hero();
    cout << "\n  " << GRAY << "¿Guardar datos antes de salir?\n\n" << RESET;
    cout << "  " << CYAN << "[1]" << RESET << "  Guardar y salir\n";
    cout << "  " << CYAN << "[2]" << RESET << "  Salir sin guardar\n";
    separador();
    cout << GRAY << "  >> " << RESET;
    int conf; cin >> conf;
    if (conf == 1) guardarTodo();

    limpiarPantalla();
    return 0;
}