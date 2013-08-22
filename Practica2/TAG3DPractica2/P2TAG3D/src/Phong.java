/**
 * Phong material.
 */
public class Phong extends Material {
	/**
	 * Diffuse color.
	 */
	public Color				diffuse;
	/**
	 * Specular color.
	 */
	public Color				specular;
	/**
	 * Specular exponent.
	 */
	public double				exponent=-1;
	
	/**
	 * Evaluate material for direct lighting.
	 * @param N Surface normal.
	 * @param L Light direction (point towards the light).
	 * @param I View direction  (point towards the surface).
	 */
	public Color computeDirectLighting(Vec3 N, Vec3 L, Vec3 I) {
		Color color = new Color();
		// Comp. Difusa
		double cosAlpha = N.dot(L); // Es independiente del punto de vista
		color.setToAdd(diffuse.scale(cosAlpha));
		
		// Comp. Especular
		Vec3 R = L.reflect(N);
		double cosBeta = I.dot(R); // Es dependiente del punto de vista y la luz
		double expCosBeta = Math.pow(cosBeta,exponent);
		color.setToAdd(specular.scale(expCosBeta));
		
		return color;
	}
	
	/**
	 * Calcula el color en funci—n de la luz y de la textura.
	 * METODO CREADO POR NOSOTROS
	 */
	public Color computeDirectLighting(Vec3 N, Vec3 L, Vec3 I, Vec3 UV){
		// Comp. Difusa
		double cosAlpha = N.dot(L); // Es independiente del punto de vista
		Vec3 uv2 = new Vec3(UV);
		uv2.x *= this.tscale.x;
		uv2.y *= this.tscale.y;
		uv2.setToAdd(this.toffset);
		Color color = this.texImg.getColorUV(uv2.x, uv2.y).scale(diffuse).scale(cosAlpha);
		
		// Comp. Especular
		Vec3 R = L.reflect(N);
		double cosBeta = I.dot(R); // Es dependiente del punto de vista y la luz
		double expCosBeta = Math.pow(cosBeta,exponent);
		color.setToAdd(specular.scale(expCosBeta));
		
		return color;
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
	 * Para las estad’sticas
	 * METODO CREADO POR NOSOTROS
	 */
	public String toString(){
		return "Phong";
	}
	
	/**
	 * Comprueba que se han introducido los par‡metros.
	 * METODO CREADO POR NOSOTROS
	 */
	public void initFromParser(){
		super.initFromParser();
		if (this.diffuse == null){
			System.err.println("Phong: Es necesario especificar el color <diffuse>");
			System.exit(1);
		}
		if (this.specular == null || this.exponent<0){
			System.err.println("Phong: Es necesario especificar los parametros especulares <specular> y <exponent>");
			System.exit(1);
		}
	}
}
