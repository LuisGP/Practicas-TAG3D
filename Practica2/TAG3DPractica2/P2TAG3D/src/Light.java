/**
 * Base class for light source.
 */
public abstract class Light {
	/**
	 * Light intensity.
	 */
	public Color				intensity;
	
	/**
	 * Default constructor.
	 */
	public Light() {
		intensity = new Color();
	}

	/**
	 * Compute light direction.
	 * @param surfacePoint Surface position.
	 */
	public abstract Vec3 computeLightDirection(Vec3 surfacePoint);
	
	/**
	 * Calcula la direcci—n para las sombras.
	 * METODO A„ADIDO POR NOSOTROS
	 */
	public abstract Vec3[] computeLightDirectionShadow(Vec3 surfacePoint);
	/**
	 * Compute light intensity.
	 * @param surfacePoint Surface position.
	 */
	public abstract Color computeLightIntensity(Vec3 surfacePoint);
	/**
	 * Compute shadow distance, i.e. max distance (in world space) a ray should 
	 * travel when checking for shadows. 
	 * @param surfacePoint Surface position.
	 */
	public abstract double computeShadowDistance(Vec3 surfacePoint);
	
	/**
	 * Comprobar par‡metros.
	 */
	public void initFromParser() {
		// Comprobar par‡metros
		if (this.intensity == null){
			System.err.println("Light: Falta la intensidad de la luz <intensity>");
			System.exit(1);
		}
	}
}
