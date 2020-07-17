import java.util.Arrays;
import java.util.concurrent.ThreadLocalRandom;

public class Block {
    public int Length;
    public int[] array;

    public Block(int Length) {
        this.Length = Length;
        this.array = new int[Length];

        for (int i = 0; i < Length; i++)
            array[i] = ThreadLocalRandom.current().nextInt(0, 2);

    }

    static public void SplitBlock(int[] original, int[] left_array, int[]  right_array){
        for (int i = 0; i < original.length / 2; i++){
            left_array[i] = original[i];
            right_array[i] = original[i + original.length / 2];
        }
    }

    static public void SdvigLeft(int[] array, int shiftBy, int[] result){
        int startFrom = 0;
        for(int  i = 0; i < array.length; ++i) {
            if (shiftBy + i < array.length) {
                result[i] = array[shiftBy + i];
            } else {
                result[i] = array[startFrom];
                ++startFrom;
            }
        }
    }

    static public void SdvigLeft_system_y(String[] array, int shiftBy, String[] result){
        int startFrom = 0;
        for(int  i = 0; i < array.length; ++i) {
            result[i] = "y[" + i + "-beta]";
        }
    }

    static public void SdvigLeft_system_k(String[] array, int shiftBy, String[] result){
        int startFrom = 0;
        for(int  i = 0; i < array.length; ++i) {
            result[i] = "k[" + i + "-beta]";
        }
    }

    public static int[] SdvigRight(int[] array, int shiftBy) {
        int[] result = new int[array.length];
        for(int k = 0; k < array.length; k++) {
            result[(k + shiftBy) % array.length] = array[k];
        }

        for (int i = 0; i < array.length; i++)
            array[i] = result[i];

        return(array);
    }

    public static String[] SdvigRight_system_x(String[] array, int shiftBy) {
        String[] result = new String[array.length];
        for(int k = 0; k < array.length; k++) {
            result[k] = "x[" + k + "+alpha]";
        }

        for (int i = 0; i < array.length; i++)
            array[i] = result[i];

        return(array);
    }

    public static String[] SdvigRight_system_l(String[] array, int shiftBy, int b) {
        String[] result = new String[array.length];
        for(int k = 0; k < array.length; k++) {
            result[k] = "l[" + b + "][" + k + "+alpha]";
        }

        for (int i = 0; i < array.length; i++)
            array[i] = result[i];

        return(array);
    }

    static public void XOR(int[] result, int[] array1, int[] array2){
        for (int i = 0; i < array1.length; i++)
            result[i] = (array1[i] + array2[i]) % 2;
    }

    static public void XOR_system(String[] result, String[] array1, String[] array2){
        for (int i = 0; i < array1.length; i++){
            if (array1[i] == "0")
                result[i] = array2[i];
            else
                if (array2[i] == "0")
            result[i] = array1[i];
            else
            result[i] = array1[i] + " ⊕ " + array2[i];
        }
    }

    static public void XOR_system_i(String[] result, String[] array1, String[] array2){
        for (int i = 0; i < array1.length; i++){
                result[i] = array1[i] + " ⊕ " + array2[i];
        }
    }

    static public void XOR_system_l(String[] result, String[] array1, String[] array2, int a){
        for (int i = 0; i < array1.length; i++){
            if (array1[i] == "0")
                result[i] = array2[i];
            else
            if (array2[i] == "0")
                result[i] = array1[i];
            else
                result[i] = array1[i] + " ⊕ " + "l[" + a + "]" + array2[i];
        }
    }

    static public int[] mod_2_n(int[] array1, int[] array2, int n){
        int[] result;
        double a1 = BinToDouble(array1);
        double a2 = BinToDouble(array2);
        a1 = (a1 + a2) % Math.pow(2, n);

        result = FromIntToBinArray((int) a1, n);
        return  result;
    }

    static public int[] mod_2_n_new(int[] array1, int[] array2){
        int[] result = new int[array1.length];

        result[array1.length - 1] = array1[array1.length - 1] + array2[array1.length - 1];
        result[array1.length - 2] = array1[array1.length - 2] + array2[array1.length - 2] + array1[array1.length - 1]*array2[array1.length - 1];
        for (int i = array1.length - 3; i > -1; i--){
            result[i] = (array1[i] + array2[i] + array1[i+1] + array2[i+1] + array1[i+1]*array2[i+1] + array1[i+1]*result[i+1] + array2[i+1]*result[i+1]) % 2;
        }
        return result;
    }

    static public String[] mod_2_n_new_system_simple_block(String[] array1, String[] array2, String[] c, int i){
        String[] result = new String[array1.length];
        StringBuffer sb = new StringBuffer();

        result[array1.length - 1] = array1[array1.length - 1] + " ⊕ " + array2[array1.length - 1];
        result[array1.length - 2] = array1[array1.length - 2] + " ⊕ " + array2[array1.length - 2] + " ⊕ " + "c_block[" + i + "][" + (array1.length - 2) + "]";
        c[array1.length - 2] = array1[array1.length - 1] + "*" + array2[array1.length - 1];

        for (int k = array1.length - 3; k > -1; k--){
            sb.delete(0, sb.length());
            sb.append(array1[k]);
            sb.append(" ⊕ ");
            sb.append(array2[k]);
            sb.append(" ⊕ ");
            sb.append("c_block[");
            sb.append(i);
            sb.append("][");
            sb.append(k);
            sb.append("]");
            result[k] = sb.toString();
            c[k] = array1[k+1] + " ⊕ " + array2[k+1] + " ⊕ " + "(" + array1[k+1] + "*" + array2[k+1] + ")" + "*" + "c_block[" + i + "][" + (k+1) + "]";
        }

        return result;
    }

    static public String[] mod_2_n_new_system_simple_key(String[] array1, String[] array2, String[] c, int i){
        String[] result = new String[array1.length];
        StringBuffer sb = new StringBuffer();

        result[array1.length - 1] = array1[array1.length - 1] + " ⊕ " + array2[array1.length - 1];
        result[array1.length - 2] = array1[array1.length - 2] + " ⊕ " + array2[array1.length - 2] + " ⊕ " + "c_key[" + i + "][" + (array1.length - 2) + "]";
        c[array1.length - 2] = array1[array1.length - 1] + "*" + array2[array1.length - 1];

        for (int k = array1.length - 3; k > -1; k--){
            sb.delete(0, sb.length());
            sb.append(array1[k]);
            sb.append(" ⊕ ");
            sb.append(array2[k]);
            sb.append(" ⊕ ");
            sb.append("c_key[");
            sb.append(i);
            sb.append("][");
            sb.append(k);
            sb.append("]");
            result[k] = sb.toString();
            c[k] = array1[k+1] + " ⊕ " + array2[k+1] + " ⊕ " + "(" + array1[k+1] + "*" + array2[k+1] + ")" + "*" + "c_key[" + i + "][" + (k+1) + "]";
        }

        return result;
    }


    static public int[] FromIntToBinArray(int a, int length){
        int[] array = new int[length];
        String s = Integer.toBinaryString(a);

        for (int k = 0; k < length; k++)
            array[k] = 0;

        for (int i = s.length() - 1, k = length - 1; i >= 0; i--, k--){
            array[k] = (int) s.charAt(i) - 48;
        }

        return array;
    }

    static public String[] FromIntToBinArray_system(int a, int length){
        String[] array = new String[length];
        String s = Integer.toBinaryString(a);

        for (int k = 0; k < length; k++)
            array[k] = "0";

        for (int i = s.length() - 1, k = length - 1; i >= 0; i--, k--){
            array[k] = String.valueOf(s.charAt(i));
        }

        return array;
    }

    static public void XOR_5(int[] result, int[] array1, int[] array2, int[] array3, int[] array4,int[] array5){
        for (int i = 0; i < array1.length; i++)
            result[i] = (array1[i] + array2[i] + array3[i] + array4[i] + array5[i]) % 2;
    }

    static public void Ampersant(int[] result, int[] array1, int[] array2){
        for (int i = 0; i < array1.length; i++){
            result[i] = array1[i] & array2[i];
        }
    }

    static public void ArrayPriravnivaniye(int[] levo, int[] pravo){
        for (int i = 0; i < levo.length; i++)
            levo[i] = pravo[i];
    }

    static public void ArrayPriravnivaniye_system(String[] levo, String[] pravo){
        for (int i = 0; i < levo.length; i++)
            levo[i] = pravo[i];
    }

    static public void Bit(int[] z0, int[] z, int bit){
        for (int i = 0; i < z.length; i++){
            if (i == bit)
                z[i] = z0[i];
            else
                z[i] = 0;
        }
    }

    static public double BinToDouble(int[] array){
        double c = 0.0;
        for (int i = 0; i < array.length; i++)
            c = c + array[i]*Math.pow(2, array.length - (i + 1));

        return c;
    }


}