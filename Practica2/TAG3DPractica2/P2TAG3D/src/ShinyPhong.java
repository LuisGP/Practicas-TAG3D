/**
 * A Phong material with a mirror reflection.
 *
 */
public class ShinyPhong extends Phong {
	/**
	 * Reflected color.
	 */
	public Color				reflection;

	/**
	 * Evaluate material mirror reflection.
	 * @param N Surface normal.
	 * @param I View direction  (point towards the surface).
	 */
	public Color computeReflection(Vec3 N, Vec3 I) {
		return reflection;
	}

	/**
	 * True if this material has mirror reflections.
	 * @param N Surface normal.
	 * @param I View direction  (point towards the surface).
	 */
	public boolean hasReflection(Vec3 N, Vec3 I) {
		return true;
	}
	
	/**
	 * Comprueba que se han introducido los par‡metros.
	 * METODO CREADO POR NOSOTROS
	 */
	public void initFromParser(){
		super.initFromParser();
		if (this.reflection == null){
			System.err.println("ShinyPhong: Es necesario especificar el grado de reflexi—n <reflection>");
			System.exit(1);
		}
	}
	
	/**
	 * METODO CREADO POR NOSOTROS
	 */
	public String toString(){
		return "ShinyPhong";
	}


}
