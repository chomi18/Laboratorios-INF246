import java.io.*;
import java.util.*;

public class LAB3_Kripper_Gonzalez {

    static int[][] matrizA, matrizB, resultado;
    static int filasA, columnasA, filasB, columnasB;

    public static void main(String[] args) throws Exception {
        File carpeta = new File("easy");
        File[] archivos = carpeta.listFiles((dir, name) -> name.matches("\\d+\\.txt"));

        if (archivos == null || archivos.length == 0) {
            System.out.println("No se encontraron archivos válidos en la carpeta easy/");
            return;
        }

        Arrays.sort(archivos, Comparator.comparingInt(f -> Integer.parseInt(f.getName().replace(".txt", ""))));

        BufferedWriter bw = new BufferedWriter(new FileWriter("salidaThread.txt"));

        for (File archivo : archivos) {
            String nombreArchivo = archivo.getName();

            try {
                leerArchivo("easy/" + nombreArchivo);

                if (columnasA != filasB) {
                    throw new IllegalArgumentException("Tamaños de las matrices no coinciden para multiplicación");
                }

                resultado = new int[filasA][columnasB];
                long inicio = System.nanoTime();

                Thread[] hilos = new Thread[filasA];
                for (int i = 0; i < filasA; i++) {
                    final int fila = i;
                    hilos[i] = new Thread(() -> multiplicarFila(fila));
                    hilos[i].start();
                }

                for (Thread hilo : hilos) {
                    hilo.join();
                }

                long fin = System.nanoTime();
                double tiempoSegundos = (fin - inicio) / 1_000_000_000.0;

                escribirResultadoEnArchivo(bw, nombreArchivo, tiempoSegundos);

            } catch (IllegalArgumentException e) {
                escribirErrorEnArchivo(bw, nombreArchivo, e.getMessage());
            } catch (Exception e) {
                escribirErrorEnArchivo(bw, nombreArchivo, "Error inesperado: " + e.getMessage());
            }
        }

        bw.close();
    }

    static void leerArchivo(String nombreArchivo) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(nombreArchivo));

        br.readLine(); // Saltar línea que indica "2"

        String[] dimsA = br.readLine().trim().split(" ");
        filasA = Integer.parseInt(dimsA[0]);
        columnasA = Integer.parseInt(dimsA[1]);

        matrizA = new int[filasA][columnasA];
        for (int i = 0; i < filasA; i++) {
            String[] fila = br.readLine().trim().split(" ");
            for (int j = 0; j < columnasA; j++) {
                matrizA[i][j] = Integer.parseInt(fila[j]);
            }
        }

        br.readLine(); // línea en blanco entre matrices

        String[] dimsB = br.readLine().trim().split(" ");
        filasB = Integer.parseInt(dimsB[0]);
        columnasB = Integer.parseInt(dimsB[1]);

        matrizB = new int[filasB][columnasB];
        for (int i = 0; i < filasB; i++) {
            String[] fila = br.readLine().trim().split(" ");
            for (int j = 0; j < columnasB; j++) {
                matrizB[i][j] = Integer.parseInt(fila[j]);
            }
        }

        br.close();
    }

    static void multiplicarFila(int fila) {
        for (int j = 0; j < columnasB; j++) {
            int suma = 0;
            for (int k = 0; k < columnasA; k++) {
                suma += matrizA[fila][k] * matrizB[k][j];
            }
            resultado[fila][j] = suma;
        }
    }

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

    static void escribirErrorEnArchivo(BufferedWriter bw, String nombreEntrada, String mensaje) throws IOException {
        bw.write(nombreEntrada);
        bw.newLine();
        bw.newLine();
        bw.write("ERROR: " + mensaje);
        bw.newLine();
        bw.newLine();
    }
}
