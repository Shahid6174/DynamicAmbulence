'use client';

import { useEffect, useState } from 'react';
import { Modal, Button, Form, Table, Badge } from 'react-bootstrap';

export default function UserAccessControl() {
  const [users, setUsers] = useState([]);
  const [showModal, setShowModal] = useState(false);
  const [newUser, setNewUser] = useState({ name: '', email: '', role: 'operator' });
  const [loading, setLoading] = useState(true);

  const fetchUsers = async () => {
    setLoading(true);
    try {
      const res = await fetch('/api/mongo?collection=Users');
      const json = await res.json();
      setUsers(json.data || []);
    } catch (err) {
      console.error('Failed to fetch users:', err);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchUsers();
  }, []);

  const handleAddUser = async () => {
    const userData = {
      ...newUser,
      status: 'active',
    };

    try {
      const res = await fetch('/api/mongo?collection=Users', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ data: userData }),
      });

      const result = await res.json();
      if (res.ok) {
        setShowModal(false);
        setNewUser({ name: '', email: '', role: 'operator' });
        fetchUsers();
      } else {
        alert('❌ Error adding user: ' + result?.error);
      }
    } catch (err) {
      console.error('Add user error:', err);
      alert('❌ Network error');
    }
  };

  const toggleStatus = async (user) => {
    const newStatus = user.status === 'active' ? 'blocked' : 'active';

    try {
      const res = await fetch('/api/mongo?collection=Users', {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          filter: { _id: typeof user._id === 'object' ? user._id.$oid || user._id : user._id },
          update: { status: newStatus },
        }),
      });

      const result = await res.json();
      if (res.ok) {
        fetchUsers();
      } else {
        alert('❌ Update failed: ' + result?.error);
      }
    } catch (err) {
      console.error(err);
      alert('❌ Network error');
    }
  };

  const handleDelete = async (user) => {
    const confirm = window.confirm(`Delete user "${user.name}"?`);
    if (!confirm) return;

    try {
      const res = await fetch('/api/mongo?collection=Users', {
        method: 'DELETE',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          filter: { _id: typeof user._id === 'object' ? user._id.$oid || user._id : user._id },
        }),
      });

      const result = await res.json();
      if (res.ok) {
        fetchUsers();
      } else {
        alert('❌ Delete failed: ' + result?.error);
      }
    } catch (err) {
      alert('❌ Network error');
      console.error(err);
    }
  };

  return (
    <div className="container-fluid py-4">
      <div className="d-flex justify-content-between align-items-center mb-4">
        <h4 className="fw-bold text-primary mb-0">
          <i className="bi bi-shield-lock-fill me-2"></i>User Access Control
        </h4>
        <Button variant="outline-primary" onClick={() => setShowModal(true)}>
          <i className="bi bi-person-plus-fill me-2"></i> Add User
        </Button>
      </div>

      <div className="card shadow-sm">
        <div className="card-body table-responsive">
          {loading ? (
            <p className="text-center">Loading...</p>
          ) : (
            <Table hover className="align-middle small">
              <thead className="table-light">
                <tr>
                  <th>#</th>
                  <th>Name</th>
                  <th>Email</th>
                  <th>Role</th>
                  <th>Status</th>
                  <th className="text-end">Actions</th>
                </tr>
              </thead>
              <tbody>
                {users.map((user, i) => (
                  <tr key={user._id || i}>
                    <td>{i + 1}</td>
                    <td>{user.name}</td>
                    <td>{user.email}</td>
                    <td>
                      <Badge bg={user.role === 'admin' ? 'danger' : user.role === 'driver' ? 'secondary' : 'info'}>
                        {user.role}
                      </Badge>
                    </td>
                    <td>
                      <Badge bg={user.status === 'active' ? 'success' : 'dark'}>{user.status}</Badge>
                    </td>
                    <td className="text-end d-flex justify-content-end gap-2">
                      <Button
                        size="sm"
                        variant={user.status === 'active' ? 'outline-danger' : 'outline-success'}
                        onClick={() => toggleStatus(user)}
                      >
                        {user.status === 'active' ? 'Block' : 'Unblock'}
                      </Button>
                      <Button
                        size="sm"
                        variant="outline-danger"
                        onClick={() => handleDelete(user)}
                      >
                        <i className="bi bi-trash"></i>
                      </Button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </Table>
          )}
        </div>
      </div>

      {/* Add User Modal */}
      <Modal show={showModal} onHide={() => setShowModal(false)} centered>
        <Modal.Header closeButton>
          <Modal.Title>Add New User</Modal.Title>
        </Modal.Header>
        <Modal.Body>
          <Form>
            <Form.Group className="mb-3">
              <Form.Label>Full Name</Form.Label>
              <Form.Control
                type="text"
                value={newUser.name}
                onChange={(e) => setNewUser({ ...newUser, name: e.target.value })}
                placeholder="Enter full name"
              />
            </Form.Group>
            <Form.Group className="mb-3">
              <Form.Label>Email Address</Form.Label>
              <Form.Control
                type="email"
                value={newUser.email}
                onChange={(e) => setNewUser({ ...newUser, email: e.target.value })}
                placeholder="Enter email"
              />
            </Form.Group>
            <Form.Group>
              <Form.Label>Role</Form.Label>
              <Form.Select
                value={newUser.role}
                onChange={(e) => setNewUser({ ...newUser, role: e.target.value })}
              >
                <option value="admin">Admin</option>
                <option value="operator">Operator</option>
                <option value="driver">Driver</option>
              </Form.Select>
            </Form.Group>
          </Form>
        </Modal.Body>
        <Modal.Footer>
          <Button variant="secondary" onClick={() => setShowModal(false)}>
            Cancel
          </Button>
          <Button variant="primary" onClick={handleAddUser}>
            Add User
          </Button>
        </Modal.Footer>
      </Modal>
    </div>
  );
}
