<%-- 
    Document   : service
    Created on : Jul 12, 2014, 1:07:38 PM
    Author     : savelazq
--%>

<%@page import="java.util.Enumeration"%>
<%@page import="org.json.simple.JSONValue"%>
<%@page contentType="text/html; charset=UTF-8"%>
<%@ page import="java.sql.*" %>
<%@page import="org.json.simple.JSONObject"%>
<%@page import="org.json.simple.JSONArray"%>
<%!
            Connection conConexion;
            Statement scSQL;
            ResultSet rsListaRegistros;
            ResultSetMetaData lsDatos;
            String macaddress;
            String name;
            String phone;
            String comments;
            int updateQuery;
        %>
<%
            String json=request.getParameter("Helmet");
            JSONObject jsonData = (JSONObject) JSONValue.parse(json);
            macaddress = (String)jsonData.get("mac_address");
            name = (String) jsonData.get("name");
            phone = (String) jsonData.get("phone");
            comments = (String) jsonData.get("comments");
            
            Class.forName("com.mysql.jdbc.Driver").newInstance();
            conConexion = DriverManager.getConnection("jdbc:mysql://localhost/edison_helmet",
                    "helmet", "helmet");
            PreparedStatement pstatement=null;
            scSQL = conConexion.createStatement();
            String queryString="INSERT INTO helmets(id,mac_address,name,phone,comments,creation_date,is_active) VALUES(DEFAULT,?,?,?,?,NOW(),1)";
            pstatement = conConexion.prepareStatement(queryString);
            pstatement.setString(1, macaddress);
            pstatement.setString(2, name);
            pstatement.setString(3, phone);
            pstatement.setString(4, comments);
            updateQuery= pstatement.executeUpdate();
            response.setCharacterEncoding("UTF-8");
            response.setContentType("application/json");
            response.setStatus(200);
            response.getWriter().println(json);
        %>
