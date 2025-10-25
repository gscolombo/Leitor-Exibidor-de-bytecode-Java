// classfiles/Example/ThrowsDemo.java
package Example;
import java.io.IOException;

public class ThrowsDemo {
    public void f() throws IOException {
        if (System.nanoTime() == 0) throw new IOException("x");
    }
}
