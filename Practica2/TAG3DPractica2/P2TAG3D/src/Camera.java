/**
 * Represents a pin-hole camera.
 * 
 * All coordinates are given in world space.
 * Note that for simplicity, this is a left-handed coordinate frame.
 * 
 */
public class Camera {
	/**
	 * Origin.
	 */
	public Vec3				origin;
	/**
	 * Right vector.
	 */
	public Vec3				x;
	/**
	 * Up vector.
	 */
	public Vec3				y;
	/**
	 * View vector.
	 */
	public Vec3				z;

	/**
	 * Field of view along the right(x) axis.
	 */
	public double				xfov;

	/**
	 * Field of view along the right(y) axis.
	 */
	public double               yfov=-1;

	/**
	 * Width of the image in pixels.
	 */
	public int					xResolution=-1;
	/**
	 * Height of the image in pixels.
	 */
	public int					yResolution=-1;
	/**
	 * Use examples-like imagepoint computing.
	 */
	public int					compatMode=1;


	/**
	 * Generate a ray through the image location.
	 * @param u Image x location in [0,1]
	 * @param v Image y location in [0,1]
	 * @return Normalized view ray
	 */
	public Ray generateRay(double u, double v) {
		// Construir el rayo que va de la c‡mara al pixel
		Vec3 dir = null;
		if (this.compatMode == 1)
			dir = this.imagePlanePoint2(u, v).sub(this.origin);
		else
			dir = this.imagePlanePoint(u, v).sub(this.origin);
		dir = dir.normalize();
		return new Ray(origin, dir);
	}

	/**
	 * Compute the image plane point in world coordinates.
	 * @param u Image x location in [0,1]
	 * @param v Image y location in [0,1]
	 * @return Image plane point.
	 */
	public Vec3 imagePlanePoint(double u, double v) {
		// devolver el punto del plano de proyecci—n por el que pasa el rayo

		// Usamos como "distancia al plano de proyecci—n"  el valor 1.0
		// A partir de eso y el ‡ngulo sacamos la posici—n del pixel relativo a XYZ de la c‡mara
		Vec3 point = new Vec3(this.z.scale(1));
		double aspect = ((double)(this.yResolution))/((double)this.xResolution);
		point.setToAdd( this.x.scale(Math.tan(this.xfov) * (2*u-1.0) ) );
		point.setToAdd( this.y.scale(Math.tan(this.xfov) * (2*v-1.0) * aspect) );
		point.z *= -1.0;

		// Movemos el vector hasta la c‡mara
		return this.origin.add(point);
	}


	/**
	 * Compute the image plane point in world coordinates.
	 * METODO PARA QUE SALGA COMO EN LOS EJEMPLOS
	 * @param u Image x location in [0,1]
	 * @param v Image y location in [0,1]
	 * @return Image plane point.
	 */
	public Vec3 imagePlanePoint2(double u, double v) {
		// devolver el punto del plano de proyecci—n por el que pasa el rayo

		// Usamos como "distancia al plano de proyecci—n"  el valor 1.0
		// A partir de eso y el ‡ngulo sacamos la posici—n del pixel respecto de 0,0,0
		Vec3 point = new Vec3(0,0,1);
		double aspect = ((double)(this.yResolution))/((double)this.xResolution);

		point.x = Math.tan(this.xfov) * 2 * point.z * (u-0.5); 
		point.y = Math.tan(this.xfov) * 2 * point.z * (v-0.5) * aspect;

		point.z = -1.0;

		// cambio de base a XYZ de la c‡mara
		Vec3 base2[] = {x,y,z}; 
		Vec3 o1 = new Vec3();
		Vec3 pointb2 = point.changeBase(base2, o1, this.origin);
		return pointb2;
	}


	/**
	 * Initialize frame from ZY. Used by parser.
	 */
	public void initFromParser() {
		// Comprobar par‡metros
		if (this.origin == null){
			System.err.println("Camera: Falta la posici—n de la c‡mara <origin>");
			System.exit(1);
		}
		if (this.z == null){
			System.err.println("Camera: Falta el vector Z de la c‡mara <z>");
			System.exit(1);
		}
		if (this.y == null){
			System.err.println("Camera: Falta el vector UP de la c‡mara <y>");
			System.exit(1);
		}
		if (this.yfov == -1){
			System.err.println("Camera: Falta el ‡ngulo de visi—n de la c‡mara <yfov>");
			System.exit(1);
		}
		if (this.xResolution == -1 || this.yResolution == -1){
			System.err.println("Camera: Falta la resoluci—n de la imagen <xResolution><yResolution>");
			System.exit(1);
		}
		
		// normalize forward
		z.setToNormalize();

		// orthonormalize the up vector
		y = y.sub(z.scale(y.dot(z))).normalize();

		// right is just the cross product
		x = new Vec3();
		x = z.cross(y);

		yfov = Math.toRadians(yfov);
		xfov = yfov * (double)xResolution / (double)yResolution;
	}
}
