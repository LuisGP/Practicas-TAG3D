/**
 * Material que produce refracci—n.
 * CLASE CREADA POR NOSOTROS
 */
public class RefractionShinyPhong extends ShinyPhong {

	/**
	 * Provoca refracci—n.
	 */
	public boolean hasRefraction(Vec3 N, Vec3 I) {
		return true;
	}
	
	/**
	 * Para las estad’sticas.
	 */
	public String toString(){
		return "RefractionShinyPhong";
	}
}
