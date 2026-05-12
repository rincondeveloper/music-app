#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#ifdef _WIN32
  #include <windows.h>
#endif
using namespace std;

// ─── ANSI ────
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
    int  id;
    char titulo[50];
    int  numeroPista;
};

struct Album {
    int     id;
    char    titulo[50];
    char    genero[30];
    int     anio;
    Cancion canciones[10];
    int     totalCanciones;
};

struct Artista {
    int    id;
    char   nombre[50];
    Album  albumes[10];
    int    totalAlbumes;
};

// ─── ARREGLO GLOBAL ──────────────────────────────────────────────────────────
const int MAX_ARTISTAS = 10;
Artista artistas[MAX_ARTISTAS];
int totalArtistas = 0;

// ─── PILA (historial — canción anterior) ─────────────────────────────────────
struct NodoPila {
    Cancion cancion;
    string  nombreArtista;
    string  nombreAlbum;
    NodoPila* siguiente;
};
NodoPila* topePila = nullptr;

void pushPila(Cancion c, string artista, string album) {
    NodoPila* nuevo = new NodoPila();
    nuevo->cancion       = c;
    nuevo->nombreArtista = artista;
    nuevo->nombreAlbum   = album;
    nuevo->siguiente     = topePila;
    topePila             = nuevo;
}

bool popPila(Cancion& c, string& artista, string& album) {
    if (!topePila) return false;
    c       = topePila->cancion;
    artista = topePila->nombreArtista;
    album   = topePila->nombreAlbum;
    NodoPila* temp = topePila;
    topePila = topePila->siguiente;
    delete temp;
    return true;
}

// ─── COLA (siguiente canción) ─────────────────────────────────────────────────
struct NodoCola {
    Cancion cancion;
    string  nombreArtista;
    string  nombreAlbum;
    NodoCola* siguiente;
};
NodoCola* frenteCola = nullptr;
NodoCola* finalCola  = nullptr;

void enqueueCola(Cancion c, string artista, string album) {
    NodoCola* nuevo = new NodoCola();
    nuevo->cancion       = c;
    nuevo->nombreArtista = artista;
    nuevo->nombreAlbum   = album;
    nuevo->siguiente     = nullptr;
    if (!finalCola) { frenteCola = finalCola = nuevo; }
    else { finalCola->siguiente = nuevo; finalCola = nuevo; }
}

bool dequeueCola(Cancion& c, string& artista, string& album) {
    if (!frenteCola) return false;
    c       = frenteCola->cancion;
    artista = frenteCola->nombreArtista;
    album   = frenteCola->nombreAlbum;
    NodoCola* temp = frenteCola;
    frenteCola = frenteCola->siguiente;
    if (!frenteCola) finalCola = nullptr;
    delete temp;
    return true;
}

// ─── CANCIÓN ACTUAL ───────────────────────────────────────────────────────────
Cancion cancionActual;
string  artistaActual = "";
string  albumActual   = "";
bool    hayReproduccion = false;

// ─── UTILIDADES ──────────────────────────────────────────────────────────────
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

// ─── ARCHIVOS ─────────────────────────────────────────────────────────────────

// Artistas longitud fija (.bin)
struct ArtistaArchivo {
    int  id;
    char nombre[50];
    int  totalAlbumes;
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

// carga artistas.bin (lonjitud fija)
void cargarArtistas() {
    ifstream f("artistas.bin", ios::binary);
    if (!f) return;
    ArtistaArchivo a;
    totalArtistas = 0;
    // guarda un struct de artistas en arreglo
    while (f.read((char*)&a, sizeof(ArtistaArchivo)) && totalArtistas < MAX_ARTISTAS) {
        artistas[totalArtistas].id = a.id;
        strncpy(artistas[totalArtistas].nombre, a.nombre, 50);
        artistas[totalArtistas].totalAlbumes = 0;
        totalArtistas++;
    }
    f.close();
}

// Álbumes delimitadores (.csv)
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

// leer albums del .csv y guardar en Artistas Albums
void cargarAlbumes() {
    ifstream f("albumes.csv");
    if (!f) return;
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

// Canciones → campos dimensionales (.txt)
void guardarCanciones() {
    ofstream f("canciones.txt");
    for (int i = 0; i < totalArtistas; i++) {
        for (int j = 0; j < artistas[i].totalAlbumes; j++) {
            for (int k = 0; k < artistas[i].albumes[j].totalCanciones; k++) {
                Cancion& c = artistas[i].albumes[j].canciones[k];
                // formato: [idArtista][idAlbum][id][len]titulo[pista]
                int lenTitulo = strlen(c.titulo);
                f << i << " " << j << " " << c.id << " "
                  << lenTitulo << " " << c.titulo << " " << c.numeroPista << "\n";
            }
        }
    }
    f.close();
}

// leer canciones del .txt y guardar en su artista
void cargarCanciones() {
    ifstream f("canciones.txt");
    if (!f) return;
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
    cout << GREEN << "\n  ✓ Datos guardados correctamente.\n" << RESET;
}

void cargarTodo() {
    cargarArtistas();
    cargarAlbumes();
    cargarCanciones();
    cout << GREEN << "\n  ✓ Datos cargados correctamente.\n" << RESET;
}

// ─── REGISTRO ────────────────────────────────────────────────────────────────
void registrarArtista() {
    if (totalArtistas >= MAX_ARTISTAS) {
        cout << RED << "\n  Límite de artistas alcanzado.\n" << RESET;
        pausar(); return;
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
        pausar(); return;
    }
    cout << "\n  Selecciona artista:\n";
    for (int i = 0; i < totalArtistas; i++)
        cout << "  [" << i+1 << "] " << artistas[i].nombre << "\n";
    cout << GRAY << "  >> " << RESET;
    int sel; cin >> sel; sel--;
    if (sel < 0 || sel >= totalArtistas) { cout << RED << "\n  Opción inválida.\n" << RESET; pausar(); return; }
    if (artistas[sel].totalAlbumes >= 10) { cout << RED << "\n  Límite de álbumes alcanzado.\n" << RESET; pausar(); return; }

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
    cout << GREEN << "\n  ✓ Álbum registrado.\n" << RESET;
    pausar();
}

void registrarCancion() {
    if (totalArtistas == 0) {
        cout << RED << "\n  ⚠ No hay artistas registrados.\n" << RESET;
        pausar(); return;
    }
    cout << "\n  Selecciona artista:\n";
    for (int i = 0; i < totalArtistas; i++)
        cout << "  [" << i+1 << "] " << artistas[i].nombre << "\n";
    cout << GRAY << "  >> " << RESET;
    int selA; cin >> selA; selA--;
    if (selA < 0 || selA >= totalArtistas) { cout << RED << "\n  Opción inválida.\n" << RESET; pausar(); return; }
    if (artistas[selA].totalAlbumes == 0) {
        cout << RED << "\n  ⚠ Este artista no tiene álbumes.\n" << RESET;
        pausar(); return;
    }
    cout << "  Selecciona álbum:\n";
    for (int j = 0; j < artistas[selA].totalAlbumes; j++)
        cout << "  [" << j+1 << "] " << artistas[selA].albumes[j].titulo << "\n";
    cout << GRAY << "  >> " << RESET;
    int selB; cin >> selB; selB--;
    if (selB < 0 || selB >= artistas[selA].totalAlbumes) { cout << RED << "\n  Opción inválida.\n" << RESET; pausar(); return; }
    if (artistas[selA].albumes[selB].totalCanciones >= 10) { cout << RED << "\n  Límite de canciones alcanzado.\n" << RESET; pausar(); return; }

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
        pausar(); return;
    }
    cout << "\n  Selecciona artista a eliminar:\n";
    for (int i = 0; i < totalArtistas; i++)
        cout << "  [" << i+1 << "] " << artistas[i].nombre << "\n";
    cout << GRAY << "  >> " << RESET;
    int sel; cin >> sel; sel--;
    if (sel < 0 || sel >= totalArtistas) { cout << RED << "\n  Opción inválida.\n" << RESET; pausar(); return; }
    for (int i = sel; i < totalArtistas - 1; i++)
        artistas[i] = artistas[i+1];
    totalArtistas--;
    cout << GREEN << "\n  ✓ Artista eliminado.\n" << RESET;
    pausar();
}

// ─── PANTALLA PRINCIPAL ───────────────────────────────────────────────────────
// Carga todas las canciones de todos los artistas/álbumes en la cola
void cargarTodasEnCola() {
    // Limpiar cola actual
    while (frenteCola) {
        NodoCola* temp = frenteCola;
        frenteCola = frenteCola->siguiente;
        delete temp;
    }
    finalCola = nullptr;

    for (int i = 0; i < totalArtistas; i++)
        for (int j = 0; j < artistas[i].totalAlbumes; j++)
            for (int k = 0; k < artistas[i].albumes[j].totalCanciones; k++)
                enqueueCola(artistas[i].albumes[j].canciones[k],
                            artistas[i].nombre,
                            artistas[i].albumes[j].titulo);
}

void mostrarTodasCanciones() {
    int num = 1;
    bool hay = false;
    for (int i = 0; i < totalArtistas; i++) {
        for (int j = 0; j < artistas[i].totalAlbumes; j++) {
            for (int k = 0; k < artistas[i].albumes[j].totalCanciones; k++) {
                hay = true;
                bool esActual = hayReproduccion &&
                    strcmp(cancionActual.titulo,
                           artistas[i].albumes[j].canciones[k].titulo) == 0;
                if (esActual)
                    cout << GREEN << "  ♪ ";
                else
                    cout << GRAY  << "  " << num << ". ";
                cout << WHITE << artistas[i].albumes[j].canciones[k].titulo
                     << GRAY << " — " << artistas[i].nombre
                     << " — " << artistas[i].albumes[j].titulo
                     << RESET << "\n";
                num++;
            }
        }
    }
    if (!hay)
        cout << GRAY << "\n  No hay canciones registradas.\n"
             << "  Ve a Registro para agregar artistas, álbumes y canciones.\n" << RESET;
}

// ─── MENÚ REGISTRO ────────────────────────────────────────────────────────────
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

// ─── BÚSQUEDAS ────────────────────────────────────────────────────────────────

// Recolecta todos los álbumes en un arreglo plano para ordenar y buscar
struct AlbumPlano {
    int     idArtista;
    int     idAlbum;
    char    titulo[50];
    char    genero[30];
    int     anio;
    int     totalCanciones;
};

int recolectarAlbumes(AlbumPlano lista[], int maxLista) {
    int total = 0;
    for (int i = 0; i < totalArtistas && total < maxLista; i++)
        for (int j = 0; j < artistas[i].totalAlbumes && total < maxLista; j++) {
            lista[total].idArtista = i;
            lista[total].idAlbum   = j;
            strncpy(lista[total].titulo, artistas[i].albumes[j].titulo, 50);
            strncpy(lista[total].genero, artistas[i].albumes[j].genero, 30);
            lista[total].anio            = artistas[i].albumes[j].anio;
            lista[total].totalCanciones  = artistas[i].albumes[j].totalCanciones;
            total++;
        }
    return total;
}

// Bubble Sort por año (más reciente → más viejo)
void bubbleSortAnio(AlbumPlano lista[], int n) {
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (lista[j].anio < lista[j+1].anio) {
                AlbumPlano temp = lista[j];
                lista[j]        = lista[j+1];
                lista[j+1]      = temp;
            }
}

// Búsqueda binaria por año (arreglo debe estar ordenado ASC)
int busquedaBinariaAnio(AlbumPlano lista[], int n, int anio) {
    // Para binaria ordenamos ASC temporalmente
    // Ya que la lista viene DESC, buscamos al revés
    int izq = 0, der = n - 1;
    while (izq <= der) {
        int mid = (izq + der) / 2;
        if (lista[mid].anio == anio) return mid;
        // lista está DESC
        else if (lista[mid].anio > anio) izq = mid + 1;
        else der = mid - 1;
    }
    return -1;
}

// Búsqueda lineal por título en arreglo
int busquedaLinealArreglo(AlbumPlano lista[], int n, const char* titulo) {
    for (int i = 0; i < n; i++)
        if (strstr(lista[i].titulo, titulo) != nullptr) return i;
    return -1;
}

// Búsqueda lineal por título en CSV
void busquedaLinealCSV(const char* titulo) {
    ifstream f("albumes.csv");
    if (!f) { cout << RED << "\n  ⚠ No se encontró albumes.csv\n" << RESET; return; }
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
    if (!encontrado) cout << RED << "\n  No se encontró \"" << titulo << "\" en el archivo.\n" << RESET;
    f.close();
}

// Carga el álbum seleccionado en la cola según tipo de lista
// (por ahora usa cola simple; las listas enlazadas se conectan aquí)
void cargarAlbumEnCola(int idArtista, int idAlbum) {
    while (frenteCola) {
        NodoCola* temp = frenteCola;
        frenteCola = frenteCola->siguiente;
        delete temp;
    }
    finalCola = nullptr;

    Album& al = artistas[idArtista].albumes[idAlbum];
    for (int k = 0; k < al.totalCanciones; k++)
        enqueueCola(al.canciones[k],
                    artistas[idArtista].nombre,
                    al.titulo);
}

void seleccionarTipoListaYReproducir(int idArtista, int idAlbum) {
    limpiarPantalla();
    hero();
    Album& al = artistas[idArtista].albumes[idAlbum];
    cout << "\n  " << CYAN << "Álbum: " << WHITE << al.titulo
         << CYAN << "  |  Artista: " << WHITE << artistas[idArtista].nombre
         << CYAN << "  |  Año: " << WHITE << al.anio << RESET << "\n\n";
    cout << "  Selecciona el tipo de lista para reproducir:\n\n";
    cout << "  " << CYAN << "[1]" << RESET << "  Simple Lineal Sin Encabezado\n";
    cout << "  " << CYAN << "[2]" << RESET << "  Simple Circular Sin Encabezado\n";
    cout << "  " << CYAN << "[3]" << RESET << "  Doble Lineal Sin Encabezado\n";
    cout << "  " << CYAN << "[4]" << RESET << "  Doble Circular Sin Encabezado\n";
    cout << "  " << CYAN << "[5]" << RESET << "  Simple Lineal Con Encabezado\n";
    cout << "  " << CYAN << "[6]" << RESET << "  Simple Circular Con Encabezado\n";
    cout << "  " << CYAN << "[7]" << RESET << "  Doble Lineal Con Encabezado\n";
    cout << "  " << CYAN << "[8]" << RESET << "  Doble Circular Con Encabezado\n";
    cout << "\n"; separador();
    cout << GRAY << "  >> " << RESET;
    int tipoLista; cin >> tipoLista;
    if (tipoLista < 1 || tipoLista > 8) {
        cout << RED << "\n  Opción inválida.\n" << RESET; pausar(); return;
    }

    // Carga canciones del álbum en cola
    cargarAlbumEnCola(idArtista, idAlbum);

    // Reproduce primera canción
    if (dequeueCola(cancionActual, artistaActual, albumActual)) {
        pushPila(cancionActual, artistaActual, albumActual);
        hayReproduccion = true;
    }

    // TODO: aquí se instanciará la lista enlazada según tipoLista
    // Por ahora confirma la selección
    cout << GREEN << "\n  ✓ Reproduciendo con lista tipo " << tipoLista << "\n" << RESET;
    pausar();
}

void mostrarResultadosYSeleccionar(AlbumPlano lista[], int n) {
    if (n == 0) { cout << RED << "\n  No se encontraron resultados.\n" << RESET; pausar(); return; }
    cout << "\n  Resultados:\n\n";
    for (int i = 0; i < n; i++)
        cout << "  [" << i+1 << "] " << WHITE << lista[i].titulo
             << GRAY << " — " << lista[i].anio
             << " — " << lista[i].genero << RESET << "\n";
    cout << "\n  Selecciona un álbum para reproducir (0 = cancelar): ";
    cout << "\n"; separador();
    cout << GRAY << "  >> " << RESET;
    int sel; cin >> sel;
    if (sel < 1 || sel > n) return;
    sel--;
    seleccionarTipoListaYReproducir(lista[sel].idArtista, lista[sel].idAlbum);
}

// ─── MENÚ BUSCAR ──────────────────────────────────────────────────────────────
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
                    // Recolectar todos del mismo año
                    AlbumPlano resultados[50];
                    int nRes = 0;
                    for (int i = 0; i < total; i++)
                        if (lista[i].anio == anio)
                            resultados[nRes++] = lista[i];
                    mostrarResultadosYSeleccionar(resultados, nRes);
                }
                break;
            }
            case 3: {
                cin.ignore();
                cout << "\n  Título a buscar: ";
                char titulo[50]; cin.getline(titulo, 50);
                AlbumPlano resultados[50]; int nRes = 0;
                for (int i = 0; i < total; i++)
                    if (strstr(lista[i].titulo, titulo) != nullptr)
                        resultados[nRes++] = lista[i];
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
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

// ─── MENÚ ARCHIVOS ────────────────────────────────────────────────────────────
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
            case 2: cargarTodo();  pausar(); break;
            case 0: break;
            default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar();
        }
    }
}

// ─── PANTALLA PRINCIPAL ───────────────────────────────────────────────────────
void pantallaPrincipal() {
    int op = -1;
    while (op != 0) {
        limpiarPantalla();
        hero();
        mostrarReproduciendo();
        cout << "\n";
        mostrarTodasCanciones();
        cout << "\n"; separador();
        cout << "  " << "   ⏮\t\t ▶\t    ⏭\n"       << RESET;
        cout << "  " << "Anterior\tPlay\t Siguiente\n"       << RESET;
        cout << "  " << CYAN << "  [A]\t\t[P]\t   [S]\n" << RESET;
        cout << DIM  << "  ─────────────────────────────────────────────\n" << RESET;
        cout << "  " << CYAN << "[1]" << RESET << WHITE << "  Registro\n"                   << RESET;
        cout << "  " << CYAN << "[2]" << RESET << WHITE << "  Buscar álbum\n"               << RESET;
        cout << "  " << CYAN << "[3]" << RESET << WHITE << "  Archivos\n"                   << RESET;
        cout << "\n  " << RED << "[0]" << RESET << RED  << "  Salir\n"                      << RESET;
        separador();
        cout << GRAY << "  >> " << RESET;

        char entrada[10];
        cin >> entrada;

        if (entrada[0] == 'P' || entrada[0] == 'p') {
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
                if (hayReproduccion) pushPila(cancionActual, artistaActual, albumActual);
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
                if (hayReproduccion) enqueueCola(cancionActual, artistaActual, albumActual);
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
                case 0: break;
                default: cout << RED << "\n  Opción inválida.\n" << RESET; pausar(); op = -1;
            }
        }
    }
}

// ─── MAIN ─────────────────────────────────────────────────────────────────────
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    // Intentar cargar datos al inicio si existen los archivos
    cargarTodo();

    pantallaPrincipal();

    // Confirmación de salida
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
    cout << CYAN << "\n  ♪  IAMMUSIC  ♪\n\n" << RESET;
    return 0;
}