package Example;

public class Main {

    public static void main(String[] args) {
        double a = Double.parseDouble(args[0]);
        double b = Double.parseDouble(args[1]);

        Complex c = new Complex(a, b);
        String c_mag_angle_repr = c.mag_angle_repr();

        System.out.println(c_mag_angle_repr);
    }
    
}