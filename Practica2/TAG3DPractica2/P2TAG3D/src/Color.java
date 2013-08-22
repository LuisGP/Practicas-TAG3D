/**
 * An RGB color.
 * 
 */
public class Color {
	/**
	 * Red channel.
	 */
	public double			r;
	/**
	 * Green channel.
	 */
	public double			g;
	/**
	 * Blue channel.
	 */
	public double			b;

	/**
	 * Default constructor.
	 */
	public Color() {
		r = 0;
		g = 0;
		b = 0;
	}
	
	/**
	 * Construct and initialize to given values.
	 * @param nr Red.
	 * @param ng Green.
	 * @param nb Blue.
	 */
	public Color(double nr, double ng, double nb) {
		r = nr;
		g = ng;
		b = nb;
	}
	
	/**
	 * Scalar multiply.
	 * Assigns the value to this color.
	 */
	public void setToScale(double s) {
		r *= s;
		g *= s;
		b *= s;
	}

	/**
	 * Scalar multiply.
	 */
	public Color scale(double s) {
		return new Color(r * s,
					     g * s,
					     b * s);
	}

	/**
	 * Component-wise color multiply.
	 * Assigns the value to this color.
	 */
	public void setToScale(Color c) {
		r *= c.r;
		g *= c.g;
		b *= c.b;
	}

	/**
	 * Component-wise color multiply.
	 */
	public Color scale(Color c) {
		return new Color(r * c.r,
					     g * c.g,
					     b * c.b);
	}

	/**
	 * Component-wise color addition.
	 * Assigns the value to this color.
	 */
	public void setToAdd(Color c) {
		r += c.r;
		g += c.g;
		b += c.b;
	}
	
	/**
	 * Component-wise color addition.
	 */
	public Color add(Color c) {
		return new Color(r + c.r,
						 g + c.g,
						 b + c.b);
	}

	/**
	 * Assignment.
	 */
	public void set(Color c) {
		r = c.r;
		g = c.g;
		b = c.b;
	}
	
	/**
	 * Return a copy of this color
	 */
	public Color copy() {
		return new Color(r,g,b);
	}
	
	/**
	 * Pack color to integrer representation for image writing.
	 */
	public int toPackedInt() {
		int ir = (int)(Math.min(Math.max(r,0),1) * 255 + 0.1);
		int ig = (int)(Math.min(Math.max(g,0),1) * 255 + 0.1);
		int ib = (int)(Math.min(Math.max(b,0),1) * 255 + 0.1);
		return (ir << 16) | (ig << 8) | (ib << 0);
	}
	
	/**
	 * Crear color a partir de su representaci—n binaria.
	 * METODO CREADO POR NOSOTROS
	 */
	public void fromPackedInt(int n) {
		this.r = ((n & 0x00FF0000L) >> 16) / 255.0;
		this.g = ((n & 0x0000FF00L) >> 8) / 255.0;
		this.b = ((n & 0x000000FFL)) / 255.0;
	}
	
	
	public String toString(){
		return ("{"+r+","+g+","+b+"}");
	}
}
