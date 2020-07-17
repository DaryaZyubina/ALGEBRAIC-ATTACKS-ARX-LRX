import java.util.Arrays;
import java.util.concurrent.ThreadLocalRandom;

public class Speck {

    private static int alpha = 7;  //8
    private static int beta = 2;   //3
    private static int m = 4;

    private static int LengtofBlock = 32;
    private static int SizePartBlock = LengtofBlock/2;
    private static int Num_Rounds = 22;

    private static int[] left_block = new int[SizePartBlock];
    private static int[] right_block = new int[SizePartBlock];


    private static String[] left_block_system = new String[SizePartBlock];
    private static String[] right_block_system = new String[SizePartBlock];
    private static String[][] i_array = new String[Num_Rounds][SizePartBlock];
    private static String[][] c_block_system = new String[Num_Rounds][SizePartBlock];

    private static int[][] key = new int[Num_Rounds][SizePartBlock];
    private static int[][] l = new int[Num_Rounds + m - 2][SizePartBlock];

    private static String[][] key_system = new String[Num_Rounds][SizePartBlock];
    private static String[][] l_system = new String[Num_Rounds + m - 2][SizePartBlock];
    private static String[][] c_key_system = new String[Num_Rounds][SizePartBlock];


    private static int[] result = new int[SizePartBlock];
    private static String[] result_system = new String[SizePartBlock];


    public static void main(String[] args) {

        String[] left_block_system = {"x[0]","x[1]","x[2]","x[3]","x[4]","x[5]","x[6]","x[7]","x[8]","x[9]","x[10]","x[11]","x[12]","x[13]","x[14]","x[15]"};
        String[] right_block_system = {"y[0]","y[1]","y[2]","y[3]","y[4]","y[5]","y[6]","y[7]","y[8]","y[9]","y[10]","y[11]","y[12]","y[13]","y[14]","y[15]"};
        //String[][] i_array = {"i[0]","i[1]","i[2]","i[3]","i[4]","i[5]","i[6]","i[7]","i[8]","i[9]","i[10]","i[11]","i[12]","i[13]","i[14]","i[15]"};


        int num_Rounds = 2;

        GenerateKey_system(key_system, l_system, i_array);
        ChangeKey_system(key_system, l_system, c_key_system, i_array);

        for (int i = 0; i < num_Rounds; i++) {
            Round_system(left_block_system, right_block_system, i, key_system, c_block_system);
        }

        PrintSystem(left_block_system, right_block_system, num_Rounds, key_system, c_block_system);



    }

    public static void PrintSystem(String[] left_block, String[] right_block, int num_Rounds, String[][] c_block_system, String[][] c_key_system){

        for (int i = 0; i < left_block.length; i++) {
            System.out.println("x[" + i + "]" + " = " + left_block[i]);
        }

        for (int i = 0; i < right_block.length; i++) {
            System.out.println("y[" + i + "]" + " = " + right_block[i]);
        }

        for (int i = 0; i < num_Rounds; i++) {
            for (int k = 0; k < SizePartBlock - 1; k++){
                System.out.println("c_block[" + i + "][" + k + "]" + " = " + c_block_system[i][k]);
            }

            for (int k = 0; k < SizePartBlock - 1; k++){
                System.out.println("c_key[" + i + "][" + k + "]" + " = " + c_key_system[i][k]);
            }

        }
    }

    public static void Round(int[] left_array, int[] right_array, int i, int[][] key){
        Block.SdvigRight(left_array, alpha);
        result = Block.mod_2_n(left_array, right_array, SizePartBlock);
        Block.XOR(left_array, result, key[i]);

        Block.SdvigLeft(right_array, beta, result);
        Block.XOR(right_array, left_array, result);
    }

    public static void Round_system(String[] left_array, String[] right_array, int i, String[][] key, String[][] c_block_system){
        Block.SdvigRight_system_x(left_array, alpha);
        result_system = Block.mod_2_n_new_system_simple_block(left_array, right_array, c_block_system[i], i);
        Block.XOR_system(left_array, result_system, key[i]);

        Block.SdvigLeft_system_y(right_array, beta, result_system);
        Block.XOR_system(right_array, left_array, result_system);
    }

    public static void GenerateKey(int[][] key, int[][] l){
       for (int k = 0; k < SizePartBlock; k++)
           key[0][k] = ThreadLocalRandom.current().nextInt(0, 2);

       for (int i = 0; i < m; i++)
           for (int k = 0; k < SizePartBlock; k++)
               l[i][k] = ThreadLocalRandom.current().nextInt(0, 2);
    }

    public static void GenerateKey_system(String[][] key, String[][] l, String[][] i_array){

        for (int k = 0; k < SizePartBlock; k++)
            key[0][k] = "k[0][" + k + "]";

        for (int i = 0; i < m; i++)
            for (int k = 0; k < SizePartBlock; k++)
                l[i][k] = "l[" + i + "][" + k + "]";

        for (int a = 0; a < Num_Rounds; a++)
            for (int k = 0; k < SizePartBlock; k++)
                i_array[a][k] = "i[" + a + "][" + k + "]";
    }

    public static void ChangeKey(int[][] key, int[][] l){

        for (int k = 0; k < Num_Rounds - 1; k++){
            int[] array_l = new int[SizePartBlock];
            Block.ArrayPriravnivaniye(array_l, l[k]);

            int[] array_k = new int[SizePartBlock];
            Block.ArrayPriravnivaniye(array_k, key[k]);

            result = Block.mod_2_n(key[k], Block.SdvigRight(array_l, alpha), SizePartBlock);
            Block.XOR(l[k + m - 1], result, Block.FromIntToBinArray(k, SizePartBlock));

            Block.SdvigLeft(array_k, beta, result);
            Block.XOR(key[k+1], result, l[k + m - 1]);

        }

    }

    public static void ChangeKey_system(String[][] key, String[][] l, String[][] c_key_system, String[][] i_array){

        for (int k = 0; k < Num_Rounds - 1; k++){

            String[] array_l = new String[SizePartBlock];
            Block.ArrayPriravnivaniye_system(array_l, l[k]);

            String[] array_k = new String[SizePartBlock];
            Block.ArrayPriravnivaniye_system(array_k, key[k]);

            result_system = Block.mod_2_n_new_system_simple_key(key[k], Block.SdvigRight_system_l(array_l, alpha, k), c_key_system[k], k);
            Block.XOR_system_i(l[k + m - 1], result_system, i_array[k]);

            Block.SdvigLeft_system_k(array_k, beta, result_system);
            Block.XOR_system_l(key[k+1], result_system, l[k + m - 1], k + m - 1);

        }

    }



}
