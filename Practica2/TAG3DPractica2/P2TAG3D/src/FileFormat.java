import javax.imageio.ImageIO;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.NamedNodeMap;

import java.awt.image.BufferedImage;
import java.io.File;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Array;
import java.lang.Class;
import java.util.StringTokenizer;

/**
 * Contains various IO utilities for parsing xml scenes and saving images.
 * 
 * The parser can load scenes in a simple XML file format.
 * The parser code is based on a Java builtin XML parser and reflection, so
 * if the classes follow some simple conventions, the parser should be able to load them
 * without changes.
 * 
 * The scene format encodes objects as XML elements.
 * The name of an XML element is the name of the variable from the parent class,
 * while its type its inferred from the parent class or specified by the
 * "type=className" attribute on the XML node.
 * To disambiguate, the root node has an implicit Scene type.
 * 
 * By convention the parse assumes that classes have public member variables named as the xml
 * methods. While this is not good engineering, we will be doing things this way for simplicity!
 * The parser expects classes to have either public variables of primitive or class types
 * or arrays of class types.
 * 
 * A simple example of a simple scene would be
 * <scene>
 *   <!-- single variable named camera -->
 *   <camera>
 *     ...
 *   </camera>
 *   
 *   <!-- array of lights -->
 *   <lights>
 *     <!-- one light in the array of type point light -->
 *     <light class="PointLight">
 *       ...
 *     </light>
 *   </lights>
 *   
 *   <!-- array of surfaces -->
 *   <surfaces>
 *     <surface class="Sphere">
 *       ...
 *     </surface>
 *     <surface class="Triangle">
 *       ...
 *     </surface>
 *   </surfaces>
 * </scene>
 * 
 * @author fabio
 */
public class FileFormat 
{
	DocumentBuilder         db;

	String                  packageNamePrefix;

	public class ParserException extends Exception {
		private static final long serialVersionUID = -2409698617654879432L;
		public ParserException(String msg, Throwable cause) {
			super(msg,cause);
		}
		public ParserException(String msg) {
			super(msg,null);
		}
	}

	protected class ParserNode {
		public String               name = null;
		public Object               id = null;
		public Object               ref = null;
		public String               className = null;
		public String               content = null;
		public ParserNode[]         children = new ParserNode[0];

		void print(String indent) {
			System.out.print(indent + name);
			if(id != null) {
				System.out.println(" id " + id);                
			}
			if(ref != null) {
				System.out.println(" ref " + ref);                
			}
			if(className != null) {
				System.out.println(" class " + className);                
			}
			System.out.println();
			if(content != null) {
				System.out.println(indent + "  " + content);
			}
			for (int c = 0; c < children.length; c++) {
				children[c].print(indent + "  ");
			}
		}
	}

	public FileFormat() throws ParserException {
		try {
			DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
			db = dbf.newDocumentBuilder();
		} catch(Exception e) {
			throw new ParserException("Problems creating XML parser",e);
		}
	}

	public Scene parseXMLScene(String filename) throws ParserException {
		ParserNode root = null;
		Document doc = null;
		try {
			doc = db.parse(filename);
		} catch (Exception e) {
			throw new ParserException("Problems creating XML document",e);
		}
		Node xmlRoot = doc.getDocumentElement();
		xmlRoot.normalize();
		root = convertXMLNode(xmlRoot);

		return (Scene)parseObject(root, Scene.class);
	}

	protected static void saveImage(String filename, ColorImage image) throws Exception {
		BufferedImage bi = new BufferedImage(
				image.xResolution, image.yResolution,
				BufferedImage.TYPE_INT_RGB);
		for(int x = 0; x < image.xResolution; x ++) {
			for (int y = 0; y < image.yResolution; y++) {
				bi.setRGB(x,image.yResolution-1-y,image.getColor(x,y).toPackedInt());
			}
		}
		ImageIO.write(bi, "PNG", new File(filename));
	}

	private ParserNode convertXMLNode(Node n) throws ParserException {
		// check if this is an element
		if(n.getNodeType() != Node.ELEMENT_NODE && n.getNodeType() != Node.TEXT_NODE) {
			throw new ParserException("Error parsing XML document: " +
					"only allows for element and text nodes: " + n);            
		}

		// create new node
		ParserNode node = new ParserNode();

		// get the node name
		node.name = n.getNodeName();

		// get the xml attributes
		NamedNodeMap att = n.getAttributes();
		if(att != null) {
			Node idN = att.getNamedItem("id");
			if(idN != null) node.id = idN.getNodeValue();
			Node refN = att.getNamedItem("ref");
			if(refN != null) node.ref = refN.getNodeValue();
			Node classN = att.getNamedItem("class");
			if(classN != null) node.className = classN.getNodeValue();
		}

		// get the number of children
		NodeList nl = n.getChildNodes();
		if(nl != null) {
			// get the content of the node content if it is a test node
			if(nl.getLength() == 1 && nl.item(0).getNodeType() == Node.TEXT_NODE) {
				node.content = nl.item(0).getNodeValue();
			} else {   
				// get all the children
				int nChild = 0;
				for(int c = 0; c < nl.getLength(); c ++) {
					if(nl.item(c).getNodeType() == Node.ELEMENT_NODE) {
						nChild ++;
					}
				}
				node.children = new ParserNode[nChild];
				int cChild = 0;
				for(int c = 0; c < nl.getLength(); c ++) {
					if(nl.item(c).getNodeType() == Node.ELEMENT_NODE) {
						node.children[cChild] = convertXMLNode(nl.item(c));
						cChild ++;
					}
				}
				node.content = null;
			}
		}

		return node;
	}

	private boolean isBuiltinType(Class c) {
		return c.equals(Integer.TYPE) ||
		c.equals(Float.TYPE) ||
		c.equals(Double.TYPE) ||
		c.equals(Vec3.class) ||
		c.equals(Color.class) ||
		c.equals(String.class);
	}

	private Object parseObject(ParserNode n, Class c) throws ParserException {
		Object ret = null;

		// determine if this is a reference
		if(n.ref != null) {
			// not implement for now!
			return null;
		}

		// determine the new class to creare
		Class nc = c;
		if(n.className != null) {
			// grab the class name
			try {
				nc = Class.forName(n.className);
			} catch(Exception e) {
				throw new ParserException("Cannot create class " + nc, e);
			}
			// check if it is an ok type
			if(!c.isAssignableFrom(nc)) {
				throw new ParserException("Cannot set " + c + " with an object of class " + nc);
			}
		}

		// create new object
		try {
			ret = nc.newInstance();
		} catch(Exception e) {
			throw new ParserException("Cannot create an object of " + nc, e);
		}

		// there should be no content for a general object
		if(n.content != null) {
			throw new ParserException("Content not allowed in generic class" + nc);
		}

		// parse all children
		for (int i = 0; i < n.children.length; i++) {
			// grab the node and its variable name
			ParserNode cn = n.children[i];
			String varName = cn.name;
			Field varField = null; 
			Class varClass = null;
			Object varValue = null;

			// check if the class as a public variable of this kind
			try {
				varField = nc.getField(varName);
				varClass = varField.getType();
			} catch(Exception e) {
				throw new ParserException("Variable " + varName + " not in " + nc, e);
			}

			// check if this is an array
			if(varClass.isArray()) {
				Class varComponentType = varClass.getComponentType();
				if(isBuiltinType(varComponentType)) {
					varValue = parseBuiltinArray(cn,varComponentType);
				} else if(varComponentType.isPrimitive()) {
					throw new ParserException("Primitive type " + varClass + " not supported");
				} else {
					varValue = parseArray(cn,varComponentType);
				}
			} else
				// check if this is class for which we have a special parser
				if(isBuiltinType(varClass)) {
					varValue = parseBuiltin(cn,varClass);
				} else 
					// check if this is an unsupported primitive
					if(varClass.isPrimitive()){
						throw new ParserException("Primitive type " + varClass + " not supported");
					} else {
						varValue = parseObject(cn,varClass);
					}

			// assign the object
			try {
				varField.set(ret,varValue);
			} catch(Exception e) {
				throw new ParserException("Cannot assign to member " + varField + 
						" of class " + nc, e);
			}
		}

		// call the parser call back if available
		try {
			Method initMethod = nc.getMethod("initFromParser", new Class[0]);
			try {
				try {
					initMethod.invoke(ret, new Object[0]);
				} catch (IllegalAccessException e) {
					e.printStackTrace();
				} catch (InvocationTargetException e) {
					e.printStackTrace();
				}
			} catch (IllegalArgumentException e) {
				e.printStackTrace();
			} 
		} catch(NoSuchMethodException e) {

		}

		// done
		return ret;
	}

	private Object parseBuiltin(ParserNode n, Class c) throws ParserException {
		Object ret = null;

		// there should be no attributes in builtin types
		if(n.id != null || n.className != null || n.ref != null) {
			throw new ParserException("Cannot have attributes on builtin types of type " + c);
		}

		// there shold be no children in builtin types
		if(n.children.length != 0) {
			throw new ParserException("Cannot have children on builtin types of type " + c);            
		}

		// check if this is a String
		if(c.equals(String.class)) {
			ret = new String(n.content);
		} else if(isBuiltinType(c)) {
			Object fa = parseBuiltinArray(n, c);
			if(Array.getLength(fa) == 1) {
				ret = Array.get(fa,0);
			} else {
				throw new ParserException("Expected builtin not array when parsing " + c);                
			}
		} else {
			throw new ParserException("Builtin type " + c + " not supported");            
		}

		return ret;
	}

	private Object parseBuiltinArray(ParserNode n, Class c) throws ParserException {
		Object ret = null;

		// there should be no attributes in builtin types
		if(n.id != null || n.className != null || n.ref != null) {
			throw new ParserException("Cannot have attributes on builtin types of type " + c);
		}

		// there shold be no children in builtin types
		if(n.children.length != 0) {
			throw new ParserException("Cannot have children on builtin types of type " + c);            
		}

		// strings are not supported in arrays yet
		if(c.equals(String.class)) {
			throw new ParserException("Arrays of strings not supported " + c);                        
		}

		// grab a double array from the data
		double[] retArray = parseDoubleArray(n.content);

		// check the type and create an array
		// primitives
		if(c.equals(Integer.TYPE) || c.equals(Float.TYPE) || c.equals(Double.TYPE)) {
			ret = Array.newInstance(c,retArray.length);
			for(int i = 0; i < retArray.length; i ++) {
				if(c.equals(Integer.TYPE)) {
					Array.setInt(ret,i,(int)retArray[i]);
				} else if(c.equals(Float.TYPE)) {
					Array.setFloat(ret,i,(float)retArray[i]);
				} else if(c.equals(Double.TYPE)) {
					Array.setDouble(ret,i,retArray[i]);                    
				}
			}
		} else
			// math types
			if(c.equals(Vec3.class) || c.equals(Color.class)) {
				int nElements = retArray.length / 3;
				ret = Array.newInstance(c,nElements);
				for(int i = 0; i < nElements; i ++) {
					if(c.equals(Vec3.class)) {
						Vec3 v = new Vec3();
						v.x = retArray[i*3+0];
						v.y = retArray[i*3+1];
						v.z = retArray[i*3+2];
						Array.set(ret,i,v);
					} else if(c.equals(Color.class)) {
						Color v = new Color();
						v.r = retArray[i*3+0];
						v.g = retArray[i*3+1];
						v.b = retArray[i*3+2];
						Array.set(ret,i,v);
					}
				}
			}            

		return ret;
	}

	private Object parseArray(ParserNode n, Class c) throws ParserException {
		Object ret = null;

		// there should be no attributes in arrays
		if(n.id != null || n.className != null || n.ref != null) {
			throw new ParserException("Cannot have attributes on arrays " + c);
		}

		// there should be no content in arrays
		if(n.content != null) {
			throw new ParserException("Cannot have content on arrays " + c);            
		}

		ret = Array.newInstance(c,n.children.length);
		for (int i = 0; i < n.children.length; i++) {
			Array.set(ret,i,parseObject(n.children[i],c));
		}

		return ret;
	}

	private double[] parseDoubleArray(String content) throws ParserException {
		// grab all the token split by space
		StringTokenizer tokenizer = new StringTokenizer(content, " ");

		int nElements = tokenizer.countTokens();
		double[] ret = new double[nElements];

		try {
			for (int i = 0; i < ret.length; i++) {
				ret[i] = Double.parseDouble(tokenizer.nextToken());
			}
		} catch(Exception e) {
			throw new ParserException("Expected number",e);
		}

		return ret;
	}  
}
