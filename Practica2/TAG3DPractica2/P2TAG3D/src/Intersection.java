/**
 * Stores data of a ray intersection.
 */

public class Intersection {
	/**
	 * Intersection point.
	 */
	public Vec3				position;
	/**
	 * Normal at intersection point.
	 */
	public Vec3				normal;
	/**
	 * Material at intersection point.
	 */
	Material				material;
	/**
	 * Ray parameter (distance along the ray if ray direction is normalized).
	 */
	public double			distance;
	/**
	 * Coordenadas de textura en el punto de intersecci—n. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public Vec3				uv;
	/**
	 * Superficie con la que se colisiona. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public Surface			object;
	/**
	 * Si es con una malla, la malla. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public Mesh				mesh;
	
	/**
	 * Default contructor.
	 */
	public Intersection() {
		position = new Vec3();
		normal = new Vec3();
		material = null;
		distance = 0;
		uv = null;
		object = null;
		mesh = null;
	}

	/**
	 * Set the intersection record to no intersection.
	 */
	public void clear() {
		normal = new Vec3();
		position = new Vec3();
		material = null;
		distance = Double.MAX_VALUE;
		uv = null;
		object = null;
		mesh = null;
	}
	
	/**
	 * Assign the internal values to the one given in the parameters. 
	 */
	public void set(Intersection i) {
		normal = new Vec3(i.normal);
		position = new Vec3(i.position);
		distance = i.distance;
		material = i.material;
		uv = i.uv;
		object = i.object;
		mesh = i.mesh;
	}
}
