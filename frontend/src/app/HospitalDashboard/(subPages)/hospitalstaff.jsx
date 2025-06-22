'use client';

import { useState } from 'react';
import { Card, Table, Button, Form, Badge } from 'react-bootstrap';

const initialStaff = [
  { name: 'Dr. Emily Carter', role: 'Physician', shift: '8 AM - 6 PM', status: 'Active' },
  { name: 'Nurse David Lee', role: 'Nurse', shift: '6 PM - 6 AM', status: 'Active' },
  { name: 'Paramedic Sarah Jones', role: 'Paramedic', shift: '8 AM - 6 PM', status: 'Active' },
  { name: 'Technician Michael Brown', role: 'Technician', shift: '6 PM - 6 AM', status: 'Active' },
  { name: 'Receptionist Olivia Green', role: 'Receptionist', shift: '8 AM - 6 PM', status: 'Active' },
];

export default function DoctorsAndStaff() {
  const [staff, setStaff] = useState(initialStaff);
  const [filter, setFilter] = useState('');

  const filteredStaff = staff.filter((s) =>
    s.name.toLowerCase().includes(filter.toLowerCase()) ||
    s.role.toLowerCase().includes(filter.toLowerCase())
  );

  return (
    <div className="container-fluid py-4">
      {/* Header */}
      <div className="d-flex justify-content-between align-items-center mb-4">
        <div>
          <h4 className="fw-bold text-dark mb-1">
            <i className="bi bi-person-vcard me-2"></i>Doctors & Staff
          </h4>
          <p className="text-muted small mb-0">
            Manage doctors, nurses, technicians, and hospital support staff.
          </p>
        </div>
        <Form.Control
          type="search"
          placeholder="Search staff by name or role"
          style={{ maxWidth: '250px' }}
          value={filter}
          onChange={(e) => setFilter(e.target.value)}
        />
      </div>

      {/* Staff Table */}
      <Card className="shadow-sm">
        <Card.Body className="table-responsive">
          <Table hover className="align-middle small">
            <thead className="table-light">
              <tr>
                <th>Name</th>
                <th>Role</th>
                <th>Shift</th>
                <th>Status</th>
                <th className="text-end">Actions</th>
              </tr>
            </thead>
            <tbody>
              {filteredStaff.length === 0 ? (
                <tr>
                  <td colSpan="5" className="text-center text-muted">
                    No staff found.
                  </td>
                </tr>
              ) : (
                filteredStaff.map((member, index) => (
                  <tr key={index}>
                    <td>{member.name}</td>
                    <td>{member.role}</td>
                    <td>{member.shift}</td>
                    <td>
                      <Badge bg={member.status === 'Active' ? 'success' : 'secondary'}>
                        {member.status}
                      </Badge>
                    </td>
                    <td className="text-end">
                      <Button variant="outline-primary" size="sm" className="me-2">
                        <i className="bi bi-pencil-square me-1"></i>Edit
                      </Button>
                      <Button
                        variant={member.status === 'Active' ? 'outline-danger' : 'outline-success'}
                        size="sm"
                      >
                        {member.status === 'Active' ? (
                          <>
                            <i className="bi bi-person-dash me-1"></i>Deactivate
                          </>
                        ) : (
                          <>
                            <i className="bi bi-person-check me-1"></i>Activate
                          </>
                        )}
                      </Button>
                    </td>
                  </tr>
                ))
              )}
            </tbody>
          </Table>
        </Card.Body>
      </Card>
    </div>
  );
}
