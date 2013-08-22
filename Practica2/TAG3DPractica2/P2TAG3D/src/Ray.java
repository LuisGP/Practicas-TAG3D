/**
 * Parametric ray.
 */
public class Ray {
	/**
	 * EPSILON used to check for minimum distance interection 
	 * (avoid numerial precision problems).
	 */
	public static double	EPSILON = 1e-6;
	
	/**
	 * Origin.
	 */
	public Vec3				origin;
	/**
	 * Direction.
	 */
	public Vec3				direction;
	/**
	 * Minimum distance allowed for intersection.
	 */
	public double			minDistance;
	/**
	 * Maximum distance allowed for intersection.
	 */
	public double			maxDistance;
	
	/**
	 * How many times this ray has bounced in the scene.
	 * Used to check for stopping criterion.
	 */
	public int				rayDepth;

	/**
	 * Indice de refracci’n actual del rayo. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public double refrIdx = 1; // Por defecto, aire
	
	/**
	 * Default constructor.
	 */
	public Ray() {
		minDistance = EPSILON;
		maxDistance = Double.MAX_VALUE;
		origin = new Vec3();
		direction = new Vec3(0,0,1);
		rayDepth = 0;
	}

	/**
	 * Contruct a ray from the given origin and direction with epsilon as
	 * minimum distance and infinity (i.e. max double) for maximum.
	 */
	public Ray(Vec3 nOrigin, Vec3 nDirection) {
		origin = new Vec3(nOrigin);
		direction = new Vec3(nDirection);
		minDistance = EPSILON;
		maxDistance = Double.MAX_VALUE;
		rayDepth = 0;
	}
	
	/**
	 * Contruct a ray from the given origin and direction with epsilon as
	 * minimum distance and infinity (i.e. max double) for maximum.
	 * Set the rayDepth to the given value.
	 */
	public Ray(Vec3 nOrigin, Vec3 nDirection, int nRayDepth) {
		this(nOrigin, nDirection);
		rayDepth = nRayDepth;
	}
    
	/**
	 * Compute the point at a given distance t along the ray.
	 */
	public Vec3 evaluate(double t) {
		return origin.add(direction.scale(t));
	}
	
	/**
	 * METODO CREADO POR NOSOTROS
	 */
	public String toString(){
		return ("O:"+origin+" D:"+this.direction);
	}
}
