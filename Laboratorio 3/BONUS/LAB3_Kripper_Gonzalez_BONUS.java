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
    static final String carpetaLectura = "medium";
    public static void main(String[] args) {
        File carpeta = new File(carpetaLectura);
        File[] archivos = carpeta.listFiles((dir, name) -> name.matches("\\d+\\.txt"));

        if (archivos == null || archivos.length == 0) {
            System.out.println("No se encontraron archivos en la carpeta bonus/");
            return;
        }

        try (BufferedWriter bw = new BufferedWriter(new FileWriter("salidaThreadBonus.txt"))) {
            // ordenar por nombre numerico
            java.util.Arrays.sort(archivos, Comparator.comparingInt(f -> Integer.parseInt(f.getName().replace(".txt", ""))));

            for (File archivo : archivos) {
                String nombreArchivo = archivo.getName();
                bw.write(nombreArchivo);
                bw.newLine();

                try {
                    List<int[][]> matrices = new ArrayList<>();
                    List<int[]> dimensiones = new ArrayList<>();
                    BufferedReader br = new BufferedReader(new FileReader(carpetaLectura + "/" + nombreArchivo));

                    int cantidad = Integer.parseInt(br.readLine().trim());
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
                        if (m < cantidad - 1) br.readLine(); // línea en blanco entre matrices
                    }
                    br.close();

                    // validación de compatibilidad
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

                    // multiplicación en cadena
                    long inicio = System.nanoTime();
                    int[][] resultado = matrices.get(0);
                    for (int i = 1; i < cantidad; i++) {
                        resultado = multiplicarMatrices(resultado, matrices.get(i));
                    }
                    long fin = System.nanoTime();
                    double tiempoSegundos = (fin - inicio) / 1_000_000_000.0;

                    // escribir resultado
                    bw.newLine();
                    for (int[] fila : resultado) {
                        for (int j = 0; j < fila.length; j++) {
                            bw.write(fila[j] + (j == fila.length - 1 ? "" : " "));
                        }
                        bw.newLine();
                    }
                    bw.newLine();
                    bw.write("tiempo = " + tiempoSegundos + " segundos\n");

                    // análisis de simetría
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

    static int[][] multiplicarMatrices(int[][] A, int[][] B) {
        int filasA = A.length;
        int columnasA = A[0].length;
        int columnasB = B[0].length;
        int[][] resultado = new int[filasA][columnasB];

        Thread[] hilos = new Thread[filasA];
        for (int i = 0; i < filasA; i++) {
            final int fila = i;
            hilos[i] = new Thread(() -> {
                for (int j = 0; j < columnasB; j++) {
                    int suma = 0;
                    for (int k = 0; k < columnasA; k++) {
                        suma += A[fila][k] * B[k][j];
                    }
                    resultado[fila][j] = suma;
                }
            });
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
