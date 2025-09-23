package Example;

public class Complex {
    double a,b;
    
    public Complex(double a, double b) {
        this.a = a;
        this.b = b;
    }

    public double magnitude() {
        return Math.sqrt(a*a + b*b);
    }

    public double angle() {
        return Math.atan(b/a);
    }

    public String mag_angle_repr() {
        double r = this.magnitude();
        double theta = this.angle();

        return String.valueOf(r) + "exp(i" + String.valueOf(theta) + ")"; 
    }
}
