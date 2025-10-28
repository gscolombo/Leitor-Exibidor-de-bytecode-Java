// Outer class
class Outer {
    private String outerMessage = "Hello from Outer class";
    
    // Inner class
    class Inner {
        public void display() {
            System.out.println("Inside Inner class");
            System.out.println("Accessing: " + outerMessage);
        }
    }
    
    // Method to create and use inner class
    public void useInner() {
        Inner inner = new Inner();
        inner.display();
    }
}