/**
 * Base class for materials
 */

public abstract class Material {
	
	/**
	 * El color de la refracci—n. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public Color 				refraction;
	/**
	 * Flag de refracci—n. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public boolean 				hasRefraction = false;
	/**
	 * Coeficiente de refracci—n. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public double 				refractionCoef;
	/**
	 * Indice de la textura. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public int 					texture=-1;
	/**
	 * Contenido de la textura. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public ColorImage 			texImg=null;
	/**
	 * Desplazamiento de la textura. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public Vec3					toffset=new Vec3();
	/**
	 * Desplazamiento de la textura. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public Vec3					tscale=new Vec3(1,1,0);
	
	/**
	 * Evaluate material for direct lighting.
	 * @param N Surface normal.
	 * @param L Light direction (point towards the light).
	 * @param I View direction  (point towards the surface).
	 */
	public abstract Color computeDirectLighting(Vec3 N, Vec3 L, Vec3 I);
	
	/**
	 * Evaluate material mirror reflection.
	 * @param N Surface normal.
	 * @param I View direction  (point towards the surface).
	 */
	public abstract boolean hasReflection(Vec3 N, Vec3 I);
	
	/**
	 * True if this material has mirror reflections.
	 * @param N Surface normal.
	 * @param I View direction  (point towards the surface).
	 */
	public abstract Color computeReflection(Vec3 N, Vec3 I);
	
	/**
	 * Calculo de la refracci—n.
	 * METODO CREADO POR NOSOTROS
	 * @param N Normal
	 * @param I Direcci—n de visualizaci—n
	 * @param r Indice de refracci—n del material "origen"
	 */
	public Color computeRefraction(Vec3 N, Vec3 I){
		return this.refraction;
	}
	
	/**
	 * Refracci—n del material actual.
	 * METODO CREADO POR NOSOTROS
	 * @return 
	 */
	public double getRefraction(Vec3 N, Vec3 I){
		return this.refractionCoef;
	}
	
	/**
	 * True si el material posee refracci—n (es translœcido).
	 * METODO CREADO POR NOSOTROS
	 */
	public boolean hasRefraction(Vec3 N, Vec3 I){
		return this.hasRefraction;
	}
	
	/**
	 * True si el material contiene una textura
	 * METODO CREADO POR NOSOTROS
	 */
	public boolean hasTexture(){
		return this.texture>=0;
	}
	
	/**
	 * Calcula el color en funci—n de la luz y de la textura.
	 * METODO CREADO POR NOSOTROS
	 * @param N Surface normal.
	 * @param L Light direction (point towards the light).
	 * @param I View direction  (point towards the surface).
	 * @param UV Coordenadas UV
	 */
	public Color computeDirectLighting(Vec3 N, Vec3 L, Vec3 I, Vec3 UV){
		throw new NotImplementedException();
	}
	
	
	/**
	 * Comprueba que se han introducido los par‡metros.
	 * METODO CREADO POR NOSOTROS
	 */
	public void initFromParser(){
		if (this.hasRefraction && this.refraction == null){
			System.err.println("Material: Es necesario especificar la refracci—n <refraction>");
			System.exit(1);
		}
	}
	
}