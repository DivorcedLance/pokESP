document.addEventListener('DOMContentLoaded', () => {
  const userForm = document.getElementById('userForm');
  const productForm = document.getElementById('productForm');
  const userList = document.getElementById('userList');
  const productList = document.getElementById('productList');

  const fetchUsers = async () => {
    try {
      const response = await fetch('/users');
      const users = await response.json();
      userList.innerHTML = '';
      users.forEach(user => {
        const li = document.createElement('li');
        li.textContent = `${user.username} (ID: ${user.userid})`;
        const deleteButton = document.createElement('button');
        deleteButton.textContent = 'Eliminar';
        deleteButton.addEventListener('click', () => deleteUser(user.userid));
        li.appendChild(deleteButton);
        userList.appendChild(li);
      });
    } catch (error) {
      console.error('Error fetching users:', error);
    }
  };

  const fetchProducts = async () => {
    try {
      const response = await fetch('/products');
      const products = await response.json();
      productList.innerHTML = '';
      products.forEach(product => {
        const li = document.createElement('li');
        li.textContent = `${product.name} (EAN: ${product.ean13}) - $${product.price} - Cantidad: ${product.amount}`;
        const deleteButton = document.createElement('button');
        deleteButton.textContent = 'Eliminar';
        deleteButton.addEventListener('click', () => deleteProduct(product.ean13));
        li.appendChild(deleteButton);
        productList.appendChild(li);
      });
    } catch (error) {
      console.error('Error fetching products:', error);
    }
  };

  const deleteUser = async (userid) => {
    try {
      await fetch(`/users/${userid}`, { method: 'DELETE' });
      fetchUsers();
    } catch (error) {
      console.error('Error deleting user:', error);
    }
  };

  const deleteProduct = async (ean13) => {
    try {
      await fetch(`/products/${ean13}`, { method: 'DELETE' });
      fetchProducts();
    } catch (error) {
      console.error('Error deleting product:', error);
    }
  };

  userForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const userid = document.getElementById('userid').value;
    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;
    try {
      await fetch('/users', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ userid, username, password }),
      });
      userForm.reset();
      fetchUsers();
    } catch (error) {
      console.error('Error adding user:', error);
    }
  });

  productForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const ean13 = document.getElementById('ean13').value;
    const name = document.getElementById('name').value;
    const price = document.getElementById('price').value;
    const amount = document.getElementById('amount').value;
    try {
      await fetch('/products', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ ean13, name, price, amount }),
      });
      productForm.reset();
      fetchProducts();
    } catch (error) {
      console.error('Error adding product:', error);
    }
  });

  fetchUsers();
  fetchProducts();
});
