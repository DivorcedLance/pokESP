const express = require('express');
const bodyParser = require('body-parser');
const dotenv = require('dotenv');
const { createClient } = require('@libsql/client');
const path = require('path');

dotenv.config();

const app = express();
const port = process.env.PORT || 3000;

const db = createClient({
  url: 'libsql://pokesp-db-divorcedlance.aws-us-west-2.turso.io',
  authToken: process.env.DB_TOKEN,
  syncUrl: null, //
});

(async () => {
  try {
    await db.execute(`
      CREATE TABLE IF NOT EXISTS products (
        ean13 TEXT PRIMARY KEY,
        name TEXT,
        price FLOAT,
        amount INTEGER
      );
    `);
  } catch (error) {
    console.error('Error al ejecutar la consulta SQL:', error);
  }
})();
(async () => {
  try {
    await db.execute(`
      CREATE TABLE IF NOT EXISTS user (
        userid TEXT PRIMARY KEY,
        password TEXT,
        username TEXT
      );
    `);
  } catch (error) {
    console.error('Error al ejecutar la consulta SQL:', error);
  }
})();

app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, 'public')));

// Rutas para la tabla products
app.post('/products', async (req, res) => {
  const { ean13, name, price, amount } = req.body;
  try {
    await db.execute({
      sql: 'INSERT INTO products (ean13, name, price, amount) VALUES (?, ?, ?, ?)',
      args: [ean13, name, price, amount],
    });
    res.send('Producto agregado');
  } catch (error) {
    console.error('Error al insertar en la base de datos:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.get('/products', async (req, res) => {
  try {
    const response = await db.execute('SELECT * FROM products');
    res.json(response.rows);
  } catch (error) {
    console.error('Error al obtener datos de la base de datos:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.get('/products/:ean13', async (req, res) => {
  const { ean13 } = req.params;
  try {
    const response = await db.execute({
      sql: 'SELECT * FROM products WHERE ean13 = ?',
      args: [ean13],
    });
    res.json(response.rows[0]);
  } catch (error) {
    console.error('Error al obtener datos de la base de datos:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.put('/products/price/:ean13', async (req, res) => {
  const { ean13 } = req.params;
  const { price } = req.body;
  try {
    await db.execute({
      sql: 'UPDATE products SET price = ? WHERE ean13 = ?',
      args: [price, ean13],
    });
    res.send('Precio actualizado');
  } catch (error) {
    console.error('Error al actualizar el precio:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.put('/products/amount/:ean13', async (req, res) => {
  const { ean13 } = req.params;
  const { amount } = req.body;
  try {
    await db.execute({
      sql: 'UPDATE products SET amount = ? WHERE ean13 = ?',
      args: [amount, ean13],
    });
    res.send('Cantidad actualizada');
  } catch (error) {
    console.error('Error al actualizar la cantidad:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.delete('/products/:ean13', async (req, res) => {
  const { ean13 } = req.params;
  try {
    await db.execute({
      sql: 'DELETE FROM products WHERE ean13 = ?',
      args: [ean13],
    });
    res.send('Producto eliminado');
  } catch (error) {
    console.error('Error al eliminar el producto:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});


// Rutas para la tabla user
app.post('/users', async (req, res) => {
  const { userid, password, username } = req.body;
  try {
    await db.execute({
      sql: 'INSERT INTO user (userid, password, username) VALUES (?, ?, ?)',
      args: [userid, password, username],
    });
    res.send('Usuario agregado');
  } catch (error) {
    console.error('Error al insertar en la base de datos:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.get('/users', async (req, res) => {
  try {
    const response = await db.execute('SELECT * FROM user');
    res.json(response.rows);
  } catch (error) {
    console.error('Error al obtener datos de la base de datos:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.get('/users/:userid', async (req, res) => {
  const { userid } = req.params;
  try {
    const response = await db.execute({
      sql: 'SELECT * FROM user WHERE userid = ?',
      args: [userid],
    });
    res.json(response.rows[0]);
  } catch (error) {
    console.error('Error al obtener datos de la base de datos:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.delete('/users/:userid', async (req, res) => {
  const { userid } = req.params;
  try {
    await db.execute({
      sql: 'DELETE FROM user WHERE userid = ?',
      args: [userid],
    });
    res.send('Usuario eliminado');
  } catch (error) {
    console.error('Error al eliminar el usuario:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.post('/users/validate', async (req, res) => {
  const { userid, password } = req.body;
  try {
    const response = await db.execute({
      sql: 'SELECT * FROM user WHERE userid = ? AND password = ?',
      args: [userid, password],
    });
    if (response.rows.length > 0) {
      res.send('Usuario válido');
    } else {
      res.status(401).send('Usuario o contraseña incorrectos');
    }
  } catch (error) {
    console.error('Error al validar el usuario:', error);
    res.status(500).send('Error al procesar la solicitud');
  }
});

app.get('/debug', async (req, res) => {
  try {
    const response = await db.execute('SELECT * FROM products');
    console.log("Debug /products:", response.rows);
    res.json(response.rows);
  } catch (e) {
    res.status(500).json({ error: e.message });
  }
});

// Inicia el servidor
app.listen(port, () => {
  console.log(`Servidor Express en ejecución en el puerto ${port}`);
});
