/**
 * A Phong material with a colored mirror reflection.
 * CLASE CREADA POR NOSOTROS
 */
public class MetalPhong extends ShinyPhong {
	/**
	 * Ponemos de color especular y reflejos el mismo que el difuso.
	 * Use by the parser.
	 */
	public void initFromParser() {
		super.initFromParser();
		if (this.reflection == null){
			System.err.println("MetalPhong: Es necesario especificar el grado de reflexi—n <reflection>");
			System.exit(1);
		}
		this.specular.setToScale(this.diffuse);
		
		this.reflection.g = this.reflection.r;
		this.reflection.b = this.reflection.r;
		this.reflection.setToScale(this.diffuse);
	}
	
	/**
	 * Calcula el color en funci—n de la luz y de la textura.
	 */
	public Color computeDirectLighting(Vec3 N, Vec3 L, Vec3 I, Vec3 UV){
		// Comp. Difusa
		double cosAlpha = N.dot(L); // Es independiente del punto de vista
		Vec3 uv2 = new Vec3(UV);
		uv2.x *= this.tscale.x;
		uv2.y *= this.tscale.y;
		uv2.setToAdd(this.toffset);
		Color color = this.texImg.getColorUV(uv2.x, uv2.y).scale(diffuse).scale(cosAlpha);
		double media = (color.r+color.g+color.b)/3.0;
		color = this.diffuse.scale(media);
		
		// Comp. Especular
		Vec3 R = L.reflect(N);
		double cosBeta = I.dot(R); // Es dependiente del punto de vista y la luz
		double expCosBeta = Math.pow(cosBeta,exponent);
		color.setToAdd(specular.scale(expCosBeta));
		
		return color;
	}
	
	/**
	 * Para las estad’sticas
	 */
	public String toString(){
		return "MetalPhong";
	}
	

}
