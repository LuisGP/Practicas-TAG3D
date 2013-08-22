/**
 * Indexed Triangle.
 * CLASE CREADA POR NOSOTROS
 */
public class TriangleInd extends Triangle {
	/**
	 * Vertex 0 index.
	 */
	public int					i0;
	/**
	 * Vertex 1 index.
	 */
	public int					i1;
	/**
	 * Vertex 2 index.
	 */
	public int					i2;
	/**
	 * calcular UVs 
	 */
	public boolean				compUVs;
	
	/**
	 * @return the i0
	 */
	public int getI0() {
		return i0;
	}
	/**
	 * @return the i1
	 */
	public int getI1() {
		return i1;
	}
	/**
	 * @return the i2
	 */
	public int getI2() {
		return i2;
	}
	/**
	 * @param i0 the i0 to set
	 */
	public void setI0(int i0) {
		this.i0 = i0;
	}
	/**
	 * @param i1 the i1 to set
	 */
	public void setI1(int i1) {
		this.i1 = i1;
	}
	/**
	 * @param i2 the i2 to set
	 */
	public void setI2(int i2) {
		this.i2 = i2;
	}
	
	/**
	 * True si el ’ndice es uno de los del tri‡ngulo
	 * @param i
	 * @return
	 */
	public boolean in(int i) {
		return ( i == this.i0 || i == this.i1 || i == this.i2 );
	}
	
	
	/**
	 * Determina si hay que calcular las coordenadas de textura.
	 */
	public boolean hasUVs(){
		return this.compUVs;
	}
	
}
