    /*
    Implementación del Juego de la Vida utilizando Programación Orientada a Objetos (POO)
    
    Conceptos Clave de POO utilizados:
    - **Encapsulamiento**: Uso de modificadores `private` y `public` para restringir el acceso a variables y métodos.
    - **Abstracción**: Definición de clases que representan conceptos del juego, como `Celula`, `Tablero` y `JuegoDeLaVida`.
    - **Herencia**: Creación de patrones a partir de la clase base `Patron` y sus clases derivadas `Planeador` y `Bloque`.
    - **Polimorfismo**: Uso de métodos en la clase base `Patron` que pueden aplicarse a diferentes patrones en clases derivadas.
    - **Constructores**: Inicialización de objetos al momento de su creación.
    - **Getters y Setters**: Métodos para acceder y modificar atributos privados de una clase.
    - **Alcance de los atributos**: Uso de `private` para restringir el acceso y `public` para permitirlo.
    - **Función Toroidal**: Manejo de los bordes del tablero con aritmética modular para hacer el espacio cíclico.
    */

    #include <iostream>
    #include <cstdlib>
    #include <ctime>
    #include <vector>

    // Control de terminal
    void mover_cursor(int x, int y) { std::cout << "\033[" << y + 1 << ";" << x + 1 << "H"; }
    void limpiar_pantalla() { std::cout << "\033[2J\033[H"; }
    void esperar(int milisegundos) { 
        for(clock_t inicio = clock(); (clock() - inicio) * 1000 / CLOCKS_PER_SEC < milisegundos;); 
    }

    // Clase que representa una célula en el juego (Encapsulamiento y Abstracción)
    class Celula {
    private:
        bool viva;  // Estado encapsulado (Modificador private restringe el acceso directo)
    public:
        Celula();  // Constructor que inicializa el estado de la célula
        bool esta_viva() const;  // Getter: Permite obtener el estado de la célula sin modificarlo
        void set_estado(bool estado);  // Setter: Permite modificar el estado de la célula
        std::string dibujar() const;  // Representación visual
    };

    Celula::Celula()  {  // Constructor por defecto inicializa la célula como muerta
        viva=false;
    }
    bool Celula::esta_viva() const { return viva; }
    void Celula::set_estado(bool estado) { viva = estado; }
    std::string Celula::dibujar() const { return viva ? "\xDB" : " "; }

    // Clase que representa el tablero del juego
    class Tablero {
    private:
        std::vector<std::vector<Celula>> celdas;  // Matriz de células
        int alto, ancho;  // Dimensiones del tablero
    public:
        Tablero();
        Tablero(int alto, int ancho);  // Constructor con parámetros para inicializar dimensiones
        void inicializar_aleatorio(double porcentaje_vivas);
        int contar_vecinos_vivos(int fila, int columna) const;  // Método que usa la función toroidal
        void actualizar_celda(int fila, int columna, bool estado);
        const Celula& obtener_celda(int fila, int columna) const;
        int get_alto() const { return alto; }  // Getter para obtener la altura
        int get_ancho() const { return ancho; }  // Getter para obtener el ancho
    };
    Tablero::Tablero() {
        // Aquí puedes inicializar un tablero vacío o por defecto
        alto = 0;
        ancho = 0;
        celdas.clear();  // Opcionalmente puedes limpiar las celdas
    }

    Tablero::Tablero(int alto, int ancho) : alto(alto), ancho(ancho) {
        celdas.resize(alto, std::vector<Celula>(ancho));
    }

    void Tablero::inicializar_aleatorio(double porcentaje_vivas) {
        srand(time(nullptr));
        for (int i = 0; i < alto; ++i)
            for (int j = 0; j < ancho; ++j)
                celdas[i][j].set_estado((rand() % 100) < (porcentaje_vivas * 100));
    }

    // Cuenta los vecinos vivos aplicando una estrategia de envoltura (bordes cíclicos - función toroidal)
    int Tablero::contar_vecinos_vivos(int fila, int columna) const {
        int vecinos = 0;
        for (int i = -1; i <= 1; ++i)
            for (int j = -1; j <= 1; ++j)
                if (!(i == 0 && j == 0))
                    vecinos += celdas[(fila + i + alto) % alto][(columna + j + ancho) % ancho].esta_viva();
        return vecinos;
    }
    void Tablero::actualizar_celda(int fila, int columna, bool estado) {
        celdas[fila][columna].set_estado(estado);
    }

    const Celula& Tablero::obtener_celda(int fila, int columna) const {
        return celdas[fila][columna];
    }
    // Representación visual de patrones


    // Clase base para patrones (Herencia y Polimorfismo)
    class Patron {
    protected:
        std::vector<std::vector<int>> patron;
    public:
        Patron() {}
        Patron(const std::vector<std::vector<int>>& _patron) ;
        virtual void aplicar(Tablero& tablero, int fila, int columna);  // Método polimórfico
    };
    Patron::Patron(const std::vector<std::vector<int>>& _patron)  {
        patron=_patron;
    }
    void Patron::aplicar(Tablero& tablero, int fila, int columna) {
        for (size_t i = 0; i < patron.size(); ++i)
            for (size_t j = 0; j < patron[i].size(); ++j)
                tablero.actualizar_celda((fila + i) % tablero.get_alto(), (columna + j) % tablero.get_ancho(), patron[i][j]);
    }

    // Clase derivada que representa un Planeador (Herencia de Patron)
    // Planeador:
    //  0 1 0
    //  0 0 1
    //  1 1 1
    class Planeador : public Patron {
    public:
        Planeador() { patron = {
            {0, 1, 0}, 
            {0, 0, 1}, 
            {1, 1, 1}
            }; 
        }
    };

    // Clase derivada que representa un Bloque (Herencia de Patron)
    // Bloque:
    //  1 1
    //  1 1
    class Bloque : public Patron {
    public:
        Bloque() { patron = {{1, 1}, {1, 1}}; }
    };

    // Clase que gestiona la simulación del juego
    class JuegoDeLaVida {
    private:
        Tablero tablero;
        int generacion;
        std::vector<std::vector<bool>> estado_anterior;

    public:
        JuegoDeLaVida(int alto, int ancho);
        void siguiente_generacion();
        void ejecutar(int generaciones, int retardo_ms);
        void dibujar();

        Tablero& get_tablero() { return tablero; }
    };

    JuegoDeLaVida::JuegoDeLaVida(int alto, int ancho)
    {
        tablero = Tablero(alto, ancho);
        generacion=0;
        estado_anterior=  std::vector<std::vector<bool>>(alto, std::vector<bool>(ancho, false));
    }

    void JuegoDeLaVida::siguiente_generacion() {
        std::vector<std::vector<bool>> nuevo_estado(tablero.get_alto(), std::vector<bool>(tablero.get_ancho(), false));
        for (int fila = 0; fila < tablero.get_alto(); ++fila)
            for (int columna = 0; columna < tablero.get_ancho(); ++columna) {
                int vecinos = tablero.contar_vecinos_vivos(fila, columna);
                bool esta_viva = tablero.obtener_celda(fila, columna).esta_viva();
                nuevo_estado[fila][columna] = (esta_viva && (vecinos == 2 || vecinos == 3)) || (!esta_viva && vecinos == 3);
            }
        for (int fila = 0; fila < tablero.get_alto(); ++fila)
            for (int columna = 0; columna < tablero.get_ancho(); ++columna)
                tablero.actualizar_celda(fila, columna, nuevo_estado[fila][columna]);
        generacion++;
    }


    void JuegoDeLaVida::dibujar() {
        mover_cursor(0,1);
        for (int fila = 0; fila < tablero.get_alto(); ++fila) {
            for (int columna = 0; columna < tablero.get_ancho(); ++columna) {
                bool actual = tablero.obtener_celda(fila, columna).esta_viva();
                if (estado_anterior[fila][columna] != actual) {
                    mover_cursor(columna, fila);
                    std::cout << tablero.obtener_celda(fila, columna).dibujar();
                }
                estado_anterior[fila][columna] = actual;
            }
        }
        mover_cursor(0, 0);
        std::cout << "Generación: " << generacion << std::endl;
    }

    void JuegoDeLaVida::ejecutar(int generaciones, int retardo_ms) {
        limpiar_pantalla();
        for (int i = 0; i < generaciones; ++i) {
            dibujar();
            siguiente_generacion();
            esperar(retardo_ms);
        }
    }
    int main() {
        JuegoDeLaVida juego(20, 40);
        Planeador planeador;
        planeador.aplicar(juego.get_tablero(), 5, 10);
        Bloque bloque;
        bloque.aplicar(juego.get_tablero(), 15, 15);
         //ejepmlo con poo
        Patron parpadeante({
            {1,0,1},
            {0,1,0},
            {0,1,0},
        });
        parpadeante.aplicar(juego.get_tablero(),10,25);
        juego.ejecutar(400, 100);
        cout<<"Hola";
        return 0;
    }
