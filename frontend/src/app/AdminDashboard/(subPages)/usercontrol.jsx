'use client';

import { useState } from 'react';
import { Modal, Button, Form, Table, Badge } from 'react-bootstrap';

const defaultUsers = [
  { id: 1, name: 'Ayush', role: 'admin', email: 'ayush@domain.com', status: 'active' },
  { id: 2, name: 'Shahid', role: 'driver', email: 'shahid@domain.com', status: 'blocked' },
];

export default function UserAccessControl() {
  const [users, setUsers] = useState(defaultUsers);
  const [showModal, setShowModal] = useState(false);
  const [newUser, setNewUser] = useState({ name: '', email: '', role: 'operator' });

  const handleAddUser = () => {
    const nextId = users.length + 1;
    setUsers([...users, { ...newUser, id: nextId, status: 'active' }]);
    setShowModal(false);
    setNewUser({ name: '', email: '', role: 'operator' });
  };

  const toggleStatus = (id) => {
    setUsers(
      users.map((user) =>
        user.id === id
          ? { ...user, status: user.status === 'active' ? 'blocked' : 'active' }
          : user
      )
    );
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
                <tr key={user.id}>
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
                  <td className="text-end">
                    <Button
                      size="sm"
                      variant={user.status === 'active' ? 'outline-danger' : 'outline-success'}
                      onClick={() => toggleStatus(user.id)}
                    >
                      {user.status === 'active' ? 'Block' : 'Unblock'}
                    </Button>
                  </td>
                </tr>
              ))}
            </tbody>
          </Table>
        </div>
      </div>

      {/* Modal */}
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
