// Outer class
class Outer {
    private String outerMessage = "Hello from Outer class";
    
    // Inner class
    class Foo {
        public void display() {
            System.out.println("Inside Foo class");
            System.out.println("Accessing: " + outerMessage);
        }
    }

    class Bar {
        public void display() {
            System.out.println("Inside Bar class");
            System.out.println("Accessing: " + outerMessage);
        }
    }
    
    
    // Methods to create and use inner classes
    public void useFoo() {
        Foo foo = new Foo();
        foo.display();
    }

    public void useBar() {
        Bar bar = new Bar();
        bar.display();
    }
}