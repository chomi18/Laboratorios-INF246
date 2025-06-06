import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Comparator;
import java.util.Arrays;

public class LAB3_Kripper_Gonzalez {
    static final String carpetaLectura = "easy"; //Aqui escribir la direccion de la carpeta donde se encuentran los archivos a analizar
    static int[][] matrizA, matrizB, resultado;
    static int filasA, columnasA, filasB, columnasB;

    public static void main(String[] args) throws Exception {
        File carpeta = new File(carpetaLectura);
        File[] archivos = carpeta.listFiles((dir, name) -> name.matches("\\d+\\.txt"));  // Arreglo con los nombres de los archivos extraidos de la carpeta
        //Si no se encontraron archivos en la carpeta
        if (archivos == null || archivos.length == 0) {
            System.out.println("No se encontraron archivos válidos en la carpeta");
            return;
        }

        Arrays.sort(archivos, Comparator.comparingInt(f -> Integer.parseInt(f.getName().replace(".txt", ""))));

        BufferedWriter bw = new BufferedWriter(new FileWriter("salidaThread.txt"));
        //Por cada archivo empieza a hacer multiplicación de matrices
        for (File archivo : archivos) {
            String nombreArchivo = archivo.getName();

            try {
                leerArchivo(carpetaLectura + "/" + nombreArchivo);
                //Si los tamaños de las matrices no coinciden lo escribe en el archivo y no hace el calculo de la multiplicación correspondiente. Pasa al siguiente archivo
                if (columnasA != filasB) {
                    throw new IllegalArgumentException("Los tamaños de las matrices no coinciden"); //Declara el error en caso de que las dimensiones de las matrices no coincidan
                }

                resultado = new int[filasA][columnasB];
                long inicio = System.nanoTime();
                //Creación de los hilos por cada fila de la matriz
                Thread[] hilos = new Thread[filasA];
                for (int i = 0; i < filasA; i++) {
                    final int fila = i;
                    hilos[i] = new Thread(() -> multiplicarFila(fila));
                    hilos[i].start();
                }
                //Cierre de los hilos
                for (Thread hilo : hilos) {
                    hilo.join();
                }

                long fin = System.nanoTime();
                double tiempoSegundos = (fin - inicio) / 1_000_000_000.0;

                escribirResultadoEnArchivo(bw, nombreArchivo, tiempoSegundos);

            } catch (IllegalArgumentException e) {
                escribirErrorEnArchivo(bw, nombreArchivo, e.getMessage()); //Escribe en el archivo el error de que las dimensiones de las matrices no coinciden
            } catch (Exception e) {
                escribirErrorEnArchivo(bw, nombreArchivo, "Error inesperado: " + e.getMessage());
            }
        }

        bw.close();
    }
    /* void leerArchivo: La función lee el archivo, guardando los datos requeridos en las variables globales. Tal como los tamaños de la matriz, las matrices, etc. 
     */
    static void leerArchivo(String nombreArchivo) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(nombreArchivo));

        br.readLine(); // Salta la primera linea
        //Lee y guarda dimensiones de la matriz A donde corresponda
        String[] dimensionesA = br.readLine().trim().split(" ");
        filasA = Integer.parseInt(dimensionesA[0]);
        columnasA = Integer.parseInt(dimensionesA[1]);
        //Lee la matriz A y va guardando los valores
        matrizA = new int[filasA][columnasA];
        for (int i = 0; i < filasA; i++) {
            String[] fila = br.readLine().trim().split(" ");
            for (int j = 0; j < columnasA; j++) {
                matrizA[i][j] = Integer.parseInt(fila[j]);
            }
        }

        br.readLine(); // línea en blanco entre matrices
        //Lee y guarda dimensiones de la matriz A donde corresponda
        String[] dimensionesB = br.readLine().trim().split(" ");
        filasB = Integer.parseInt(dimensionesB[0]);
        columnasB = Integer.parseInt(dimensionesB[1]);
        //Lee la matriz B y va guardando los valores
        matrizB = new int[filasB][columnasB];
        for (int i = 0; i < filasB; i++) {
            String[] fila = br.readLine().trim().split(" ");
            for (int j = 0; j < columnasB; j++) {
                matrizB[i][j] = Integer.parseInt(fila[j]);
            }
        }

        br.close();
    }
    /* void multiplicarFila: Es la función que se encarga de multiplicar una fila de la matriz como corresponde. Guarda el resultado en la variable global resultado
     */
    static void multiplicarFila(int fila) {
        for (int i = 0; i < columnasB; i++) {
            int suma = 0;
            for (int j = 0; j < columnasA; j++) {
                suma += matrizA[fila][j] * matrizB[j][i];
            }
            resultado[fila][i] = suma;
        }
    }
    /*
     * void escribirResultadoEnArchivo: Se encarga de escribir el resultado obtenido de la multiplicación de matrices, así como también los segundos que se demoró en realizarla.
     */
    static void escribirResultadoEnArchivo(BufferedWriter bw, String nombreEntrada, double tiempoSegundos) throws IOException {
        bw.write(nombreEntrada);
        bw.newLine();
        bw.newLine();

        for (int i = 0; i < filasA; i++) {
            for (int j = 0; j < columnasB; j++) {
                bw.write(resultado[i][j] + (j == columnasB - 1 ? "" : " "));
            }
            bw.newLine();
        }

        bw.newLine();
        bw.write("tiempo = " + tiempoSegundos + " segundos");
        bw.newLine();
        bw.newLine();
    }
    /*
     * void escribirErrorEnArchivo: En caso de que haya un error (como dimensiones incorrectas de matrices) esta función se encarga de imprimir un mensaje para el error correspondiente.
     */
    static void escribirErrorEnArchivo(BufferedWriter bw, String nombreEntrada, String mensaje) throws IOException {
        bw.write(nombreEntrada);
        bw.newLine();
        bw.newLine();
        bw.write("ERROR: " + mensaje);
        bw.newLine();
        bw.newLine();
    }
}
