import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

public class LAB3_Kripper_Gonzalez_BONUS {
    static final String carpetaLectura = "medium"; //Aqui escribir la direccion de la carpeta donde se encuentran los archivos a analizar
    public static void main(String[] args) {
        File carpeta = new File(carpetaLectura);
        File[] archivos = carpeta.listFiles((_, name) -> name.matches("\\d+\\.txt")); // Arreglo con los nombres de los archivos extraidos de la carpeta
        //Si no se encontraron archivos en la carpeta
        if (archivos == null || archivos.length == 0) {
            System.out.println("No se encontraron archivos en la carpeta");
            return;
        }
        //Crea el buffer de escritura para la salida
        try (BufferedWriter bw = new BufferedWriter(new FileWriter("salidaThreadBonus.txt"))) {
    
            java.util.Arrays.sort(archivos, Comparator.comparingInt(f -> Integer.parseInt(f.getName().replace(".txt", "")))); 
            //Por cada archivo empieza a extraer datos y ejecutar lo correspondiente
            for (File archivo : archivos) {
                String nombreArchivo = archivo.getName();
                bw.write(nombreArchivo);
                bw.newLine();

                try {
                    //Se declaran listas para almacenar las n matrices del archivo, y otra para almacenar las n dimensiones de dichas matrices
                    List<int[][]> matrices = new ArrayList<>();
                    List<int[]> dimensiones = new ArrayList<>();
                    BufferedReader br = new BufferedReader(new FileReader(carpetaLectura + "/" + nombreArchivo));
                    //Obtener cantidad de matrices
                    int cantidad = Integer.parseInt(br.readLine().trim());
                    //Por cada matriz obtiene sus valores y dimensiones
                    for (int m = 0; m < cantidad; m++) {
                        String[] dims = br.readLine().trim().split(" ");
                        int filas = Integer.parseInt(dims[0]);
                        int columnas = Integer.parseInt(dims[1]);
                        int[][] matriz = new int[filas][columnas];
                        for (int i = 0; i < filas; i++) {
                            String[] fila = br.readLine().trim().split(" ");
                            for (int j = 0; j < columnas; j++) {
                                matriz[i][j] = Integer.parseInt(fila[j]);
                            }
                        }
                        matrices.add(matriz);
                        dimensiones.add(new int[]{filas, columnas});
                        if (m < cantidad - 1) br.readLine(); //Linea en blanco entre matrices
                    }
                    br.close();

                    //Validar que las dimensiones sean compatibles para multiplicación
                    boolean compatible = true;
                    for (int i = 0; i < cantidad - 1; i++) {
                        if (dimensiones.get(i)[1] != dimensiones.get(i + 1)[0]) {
                            compatible = false;
                            break;
                        }
                    }

                    if (!compatible) {
                        bw.write("\nERROR: Tamaños incompatibles para multiplicación en cadena\n\n");
                        continue;
                    }

                    //Se hace la multiplicación de las matrices
                    long inicio = System.nanoTime();
                    int[][] resultado = matrices.get(0);
                    for (int i = 1; i < cantidad; i++) {
                        resultado = multiplicarMatrices(resultado, matrices.get(i)); 
                        //Multiplicación entre la matriz resultante y la siguiente a multiplicar. Para la iteracion 0 la matriz resultante es la primera del archivo.
                    }
                    long fin = System.nanoTime();
                    double tiempoSegundos = (fin - inicio) / 1_000_000_000.0;

                    //Escribe el resultado de la multiplicación
                    bw.newLine();
                    for (int[] fila : resultado) {
                        for (int j = 0; j < fila.length; j++) {
                            bw.write(fila[j] + (j == fila.length - 1 ? "" : " "));
                        }
                        bw.newLine();
                    }
                    bw.newLine();
                    bw.write("tiempo = " + tiempoSegundos + " segundos\n");

                    //Analisis de simetría
                    if (esSimetrica(resultado)) {
                        bw.write("simetrica = SI \n");
                    } else {
                        bw.write("simetrica = NO \n");
                    }

                    bw.newLine();

                } catch (Exception e) {
                    bw.write("\nERROR: " + e.getMessage() + "\n\n");
                }
            }
        } catch (IOException e) {
            System.out.println("Error al escribir salidaThreadBonus.txt: " + e.getMessage());
        }
    }
    /*
     * int[][] multiplicarMatrices: Cómo parámetro recibe dos matrices, y procede a realizar la multiplicación entre ellas dos. 
     *                              La implementación de hilos se encuentra dentro de esta función. Retorna la matriz resultante de la multiplicación correspondiente
     */
    static int[][] multiplicarMatrices(int[][] matrizA, int[][] matrizB) {
        int filasA = matrizA.length;
        int columnasB = matrizB[0].length;
        int[][] resultado = new int[filasA][columnasB];

        Thread[] hilos = new Thread[filasA];
        for (int i = 0; i < filasA; i++) {
            final int fila = i;
            hilos[i] = new Thread(() -> multiplicarFila(matrizA, matrizB, resultado, fila));
            hilos[i].start();
        }

        for (Thread t : hilos) {
            try {
                t.join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }

        return resultado;
    }
    /*
     * void multiplicarFila: Recibe las dos matrices a multiplicar, asi como la variable resultado y la fila en la que se encuentra. 
     *                       Calcula la fila correspondiente de la matriz resultante.
     */
    static void multiplicarFila(int[][] matrizA, int[][] matrizB, int[][] resultado, int fila) {
        int columnasA = matrizA[0].length;
        int columnasB = matrizB[0].length;
        
        for (int j = 0; j < columnasB; j++) {
            int suma = 0;
            for (int k = 0; k < columnasA; k++) {
                suma += matrizA[fila][k] * matrizB[k][j];
            }
            resultado[fila][j] = suma;
        }
    }
    /*
     * boolean esSimetrica: Recibe como parámetro una matriz y evalúa si la matriz original es igual a la traspuesta. 
     *                      Retorna el valor correspondiente para cada caso.
     */
    static boolean esSimetrica(int[][] matriz) {
        int filas = matriz.length;
        int columnas = matriz[0].length;
        if (filas != columnas) return false;

        for (int i = 0; i < filas; i++) {
            for (int j = 0; j < columnas; j++) {
                if (matriz[i][j] != matriz[j][i]) {
                    return false;
                }
            }
        }
        return true;
    }
}
