/**
 * Color image represented as coor matrix.
 * 
 */
public class ColorImage {
	/**
	 * Colors
	 */
	private Color[] 			colors;

	/**
	 * Image width.
	 */
	public int					xResolution;
	/**
	 * Image height.
	 */
	public int					yResolution;
	/**
	 * Filtrar lineal al realizar el fetching. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public boolean				filter=false;
	
	/**
	 * Create an empty image of the given resolution.
	 */
	public ColorImage(int nxResolution, int nyResolution) {
		xResolution = nxResolution;
		yResolution = nyResolution;
		colors = new Color[xResolution*yResolution];
		for(int x = 0; x < xResolution; x ++) {
			for (int y = 0; y < yResolution; y++) {
				colors[idx(x,y)] = new Color(0,0,0);
			}
		}
	}

	/**
	 * Get coor at coordinate (x,y).
	 */
	public Color getColor(int x, int y) {
		return colors[idx(x,y)];
	}

	/**
	 * Set coor at coordinate (x,y).
	 */
	public void setColor(int x, int y, Color c) {
		colors[idx(x,y)].set(c);
	}

	/**
	 * Finds the image index
	 */
	private int idx(int x, int y) {
		return y*xResolution+x;
	}


	/**
	 * Devuelve el color a partir de las coordenadas UV.
	 * METODO CREADO POR NOSOTROS
	 * @param u
	 * @param v
	 * @return
	 */
	public Color getColorUV(double u, double v){
		double factores[] = new double[4];
		double factorx = 0.0;
		double factory = 0.0;
		int u1=0,u2=0,v1=0,v2=0;
		Color colores[] = new Color[4];
		
		int x,y;
		if (u==1){
			x = this.xResolution-1;
			factorx = 1.0;
		}else{
			double temp = u*this.xResolution;
			x = (int)(temp) % (xResolution);
			factorx = Math.abs(temp);
			factorx = factorx-Math.floor(factorx);
			if (x<0){
				x = xResolution+x;
				factorx = 1.0 - factorx;
			}else{
				u1 = x;
				u2 = (x+1) % xResolution;
			}
		}
		if (v==1){
			y = this.yResolution-1;
			factory = 1.0;
		}else{
			double temp = v*this.yResolution;
			y = (int)(temp) % (yResolution);
			factory = Math.abs(temp);
			factory = factory-Math.floor(factory);
			if (y<0){ 
				y = yResolution+y;
				factory = 1.0 - factory;
				v1 = y;
				v2 = (y+1) % yResolution;
			}else{
				v1 = y;
				v2 = (y+1) % yResolution;
			}
		}

		if (!filter){
			return this.getColor(x, y);
		}else{ // Interpolaci—n lineal
			u1 = x;
			u2 = (x+1) % xResolution;
			v1 = y;
			v2 = (y+1) % yResolution;
			// aportaci—n de cada texel
			factores[0] = (1.0 - factorx) * (1.0 - factory);
			factores[1] = (1.0 - factorx) * factory;
			factores[2] = factorx * (1.0 - factory);
			factores[3] = factorx * factory;
			// fetching
			colores[0] = this.getColor(u1, v1);
			colores[1] = this.getColor(u1, v2);
			colores[2] = this.getColor(u2, v1);
			colores[3] = this.getColor(u2, v2);
			// combinaci—n
			Color c = new Color();
			c.setToAdd(colores[0].scale(factores[0]));
			c.setToAdd(colores[1].scale(factores[1]));
			c.setToAdd(colores[2].scale(factores[2]));
			c.setToAdd(colores[3].scale(factores[3]));
			return c;
		}
	}
}
