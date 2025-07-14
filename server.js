const express = require('express');
const bodyParser = require('body-parser');
const dotenv = require('dotenv');
const { createClient } = require('@libsql/client');
const path = require('path');

dotenv.config();

const app = express();
const port = process.env.PORT || 3000;

// --- Inicializa base de datos (puedes cambiar a solo nube si quieres) ---
const db = createClient({
  url: "file:local.db",
  syncUrl: process.env.TURSO_URL,
  authToken: process.env.DB_TOKEN,
  syncInterval: 60000
});

// --- Migraciones iniciales ---
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
    await db.execute(`
      CREATE TABLE IF NOT EXISTS user (
        userid TEXT PRIMARY KEY,
        password TEXT,
        username TEXT
      );
    `);
    console.log("Tablas inicializadas correctamente.");
  } catch (error) {
    console.error('❌ Error al crear tablas:', error);
    // Considera terminar el proceso si no se pueden crear las tablas
    process.exit(1);
  }
})();

app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, 'public')));

// --- Función robusta para logging de errores ---
function logError(location, err, req) {
  console.error(`❌ [${location}]`, {
    message: err.message,
    stack: err.stack,
    path: req?.path,
    method: req?.method,
    body: req?.body,
    params: req?.params,
    query: req?.query,
  });
}

// --- Endpoints para productos ---

app.post('/products', async (req, res, next) => {
  const { ean13, name, price, amount } = req.body;
  try {
    await db.execute({
      sql: 'INSERT INTO products (ean13, name, price, amount) VALUES (?, ?, ?, ?)',
      args: [ean13, name, price, amount],
    });
    res.send('Producto agregado');
  } catch (error) {
    logError('POST /products', error, req);
    if (error.message?.includes("UNIQUE constraint failed")) {
      return res.status(409).send('Ya existe un producto con ese EAN13');
    }
    next(error);
  }
});

app.get('/products', async (req, res, next) => {
  try {
    const response = await db.execute('SELECT * FROM products');
    res.json(response.rows);
  } catch (error) {
    logError('GET /products', error, req);
    next(error);
  }
});

app.get('/products/:ean13', async (req, res, next) => {
  const { ean13 } = req.params;
  try {
    const response = await db.execute({
      sql: 'SELECT * FROM products WHERE ean13 = ?',
      args: [ean13],
    });
    if (response.rows.length === 0) {
      return res.status(404).send('Producto no encontrado');
    }
    res.json(response.rows[0]);
  } catch (error) {
    logError('GET /products/:ean13', error, req);
    next(error);
  }
});

app.put('/products/price/:ean13', async (req, res, next) => {
  const { ean13 } = req.params;
  const { price } = req.body;
  try {
    const result = await db.execute({
      sql: 'UPDATE products SET price = ? WHERE ean13 = ?',
      args: [price, ean13],
    });
    if (result.rowsAffected === 0) {
      return res.status(404).send('Producto no encontrado');
    }
    res.send('Precio actualizado');
  } catch (error) {
    logError('PUT /products/price/:ean13', error, req);
    next(error);
  }
});

app.put('/products/amount/:ean13', async (req, res, next) => {
  const { ean13 } = req.params;
  const { amount } = req.body;
  try {
    const result = await db.execute({
      sql: 'UPDATE products SET amount = ? WHERE ean13 = ?',
      args: [amount, ean13],
    });
    if (result.rowsAffected === 0) {
      return res.status(404).send('Producto no encontrado');
    }
    res.send('Cantidad actualizada');
  } catch (error) {
    logError('PUT /products/amount/:ean13', error, req);
    next(error);
  }
});

app.delete('/products/:ean13', async (req, res, next) => {
  const { ean13 } = req.params;
  try {
    const result = await db.execute({
      sql: 'DELETE FROM products WHERE ean13 = ?',
      args: [ean13],
    });
    if (result.rowsAffected === 0) {
      return res.status(404).send('Producto no encontrado');
    }
    res.send('Producto eliminado');
  } catch (error) {
    logError('DELETE /products/:ean13', error, req);
    next(error);
  }
});

// --- Endpoints para usuarios ---

app.post('/users', async (req, res, next) => {
  const { userid, password, username } = req.body;
  try {
    await db.execute({
      sql: 'INSERT INTO user (userid, password, username) VALUES (?, ?, ?)',
      args: [userid, password, username],
    });
    res.send('Usuario agregado');
  } catch (error) {
    logError('POST /users', error, req);
    if (error.message?.includes("UNIQUE constraint failed")) {
      return res.status(409).send('Ya existe un usuario con ese UserID');
    }
    next(error);
  }
});

app.get('/users', async (req, res, next) => {
  try {
    const response = await db.execute('SELECT * FROM user');
    res.json(response.rows);
  } catch (error) {
    logError('GET /users', error, req);
    next(error);
  }
});

app.get('/users/:userid', async (req, res, next) => {
  const { userid } = req.params;
  try {
    const response = await db.execute({
      sql: 'SELECT * FROM user WHERE userid = ?',
      args: [userid],
    });
    if (response.rows.length === 0) {
      return res.status(404).send('Usuario no encontrado');
    }
    res.json(response.rows[0]);
  } catch (error) {
    logError('GET /users/:userid', error, req);
    next(error);
  }
});

app.delete('/users/:userid', async (req, res, next) => {
  const { userid } = req.params;
  try {
    const result = await db.execute({
      sql: 'DELETE FROM user WHERE userid = ?',
      args: [userid],
    });
    if (result.rowsAffected === 0) {
      return res.status(404).send('Usuario no encontrado');
    }
    res.send('Usuario eliminado');
  } catch (error) {
    logError('DELETE /users/:userid', error, req);
    next(error);
  }
});

app.post('/users/validate', async (req, res, next) => {
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
    logError('POST /users/validate', error, req);
    next(error);
  }
});

// --- Endpoint de debug (solo para desarrollo) ---
app.get('/debug', async (req, res, next) => {
  try {
    const response = await db.execute('SELECT * FROM products');
    console.log("Debug /products:", response.rows);
    res.json(response.rows);
  } catch (error) {
    logError('GET /debug', error, req);
    next(error);
  }
});

// --- Middleware de manejo global de errores ---
app.use((err, req, res, next) => {
  // Aquí puedes mejorar el mensaje para el usuario final
  if (process.env.NODE_ENV === 'development') {
    // En desarrollo, muestra más detalles
    return res.status(500).json({
      error: err.message,
      stack: err.stack,
    });
  }
  // En producción, mensaje genérico
  res.status(500).json({ error: 'Ocurrió un error inesperado. Intenta de nuevo más tarde.' });
});

// --- Inicializa servidor ---
app.listen(port, () => {
  console.log(`Servidor Express en ejecución en el puerto ${port}`);
});
