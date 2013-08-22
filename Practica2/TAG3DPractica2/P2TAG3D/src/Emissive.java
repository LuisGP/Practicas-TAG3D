/**
 * Material emisivo. No es susceptible a las luces.
 * CLASE CREADA POR NOSOTROS
 */
public class Emissive extends Material {
	/**
	 * Diffuse color.
	 */
	public Color				diffuse;

	/**
	 * Evaluate material for direct lighting.
	 * @param N Surface normal.
	 * @param L Light direction (point towards the light).
	 * @param I View direction  (point towards the surface).
	 */
	public Color computeDirectLighting(Vec3 N, Vec3 L, Vec3 I) {
		return diffuse;
	}

	/**
	 * Calcula el color en funci—n de la luz y de la textura.
	 */
	public Color computeDirectLighting(Vec3 N, Vec3 L, Vec3 I, Vec3 UV){
		Vec3 uv2 = new Vec3(UV);
		uv2.x *= this.tscale.x;
		uv2.y *= this.tscale.y;
		uv2.setToAdd(this.toffset);
		return this.texImg.getColorUV(uv2.x, uv2.y).scale(diffuse);
	}

	/**
	 * Evaluate material mirror reflection.
	 * @param N Surface normal.
	 * @param I View direction  (point towards the surface).
	 */
	public Color computeReflection(Vec3 N, Vec3 I) {
		return diffuse;
	}

	/**
	 * True if this material has mirror reflections.
	 * @param N Surface normal.
	 * @param I View direction  (point towards the surface).
	 */
	public boolean hasReflection(Vec3 N, Vec3 I) {
		return false;
	}

	/**
	 * Para las estad’sticas.
	 */
	public String toString(){
		return "Emissive";
	}
	
	/**
	 * Comprueba que se han introducido los par‡metros.
	 */
	public void initFromParser(){
		super.initFromParser();
		if (this.diffuse == null){
			System.err.println("Emission: Es necesario especificar el color <diffuse>");
			System.exit(1);
		}
	}
	
}
